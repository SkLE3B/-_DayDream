#include "FbxLoader.h"
#include <cassert>

using namespace DirectX;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string FbxLoader::baseDirectory = "Resources/models/";

const std::string FbxLoader::defaultTextureFileName = "white1x1.png";

FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}

void FbxLoader::ComvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src)
{
    //行
    for (int i = 0; i < 4; i++)
    {
        //列
        for (int j = 0; j < 4; j++)
        {
            //1要素コピー
            dst->r[i].m128_f32[j] = (float)src.Get(i, j);
        }
    }
}

void FbxLoader::Initialize(ID3D12Device* device)
{
    //最初期化チェック  
    assert(fbxManager == nullptr);
    //引数からメンバ変数に代入
    this->device = device;
    //FBXマネージャーの生成
    fbxManager = FbxManager::Create();
    //FBXマネージャの入出力設定
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager,IOSROOT);
    fbxManager->SetIOSettings(ios);
    //FBXインポーターの生成
    fbxImporter = FbxImporter::Create(fbxManager,"");
}

Model* FbxLoader::LoadModelFromFile(const string& modelname)
{
    //モデルと同じ名前のフォルダから読み込む
    const string directoryPath = baseDirectory + modelname + "/";
    //拡張子fbxを付加
    const string fileName = modelname + ".fbx";
    //連結してプルパスを得る
    const string fullpath = directoryPath + fileName;

    //ファイル名を指定してFbxファイルを読み込む
    if (!fbxImporter->Initialize(fullpath.c_str(),
        -1,fbxManager->GetIOSettings()))
    {
        assert(0);
    }

    //シーンを生成
    FbxScene* fbxScene =
        FbxScene::Create(fbxManager, "fbxScene");
    //ファイルからロードしたFbxの情報をシーンにインポート
    fbxImporter->Import(fbxScene);

    //モデル生成
    Model* model = new Model();
    model->name = modelname;
    //FBXノードの数を取得
    int nodeCount = fbxScene->GetNodeCount();
    //あらかじめ必要分のメモリを確保することで、アドレスがずれるのを予防
    model->nodes.reserve(nodeCount);
    //ルートノードから順に解析してモデルに流し込む
    ParseNodeRecursive(model, fbxScene->GetRootNode());
    
    model->fbxScene = fbxScene;

    //バッファ生成
    model->CreateBuffers(device);

    return model;
}

void FbxLoader::ParseNodeRecursive(Model* model, FbxNode* fbxNode,Node* parent)
{
    //モデルにノードを追加
    model->nodes.emplace_back();
    Node& node = model->nodes.back();
    //ノード名を取得
    string name = fbxNode->GetName();
    
    //FBXノードのローカル移動情報
    FbxDouble3 rotation = fbxNode->LclRotation.Get();
    FbxDouble3 scaling = fbxNode->LclScaling.Get();
    FbxDouble3 translation = fbxNode->LclTranslation.Get();

    //形式変換して代入
    node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
    node.scaling = { (float)scaling[0],(float)scaling[1],(float)scaling[2],0.0f };
    node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };

    //スケール、回転、平行移動行列の計算
    XMMATRIX matScaling, matRotation, matTranslation;
    matScaling = XMMatrixScalingFromVector(node.scaling);
    matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
    matTranslation = XMMatrixTranslationFromVector(node.translation);

    //ローカル変形行列の計算
    node.transform = XMMatrixIdentity();
    node.transform *= matScaling;    //ワールド行列にスケーリングを反映
    node.transform *= matRotation;   //ワールド行列に回転を反映
    node.transform *= matTranslation;//ワールド行列に平行移動を反映

    //FBXノードの情報を解析してノードに記録(Todo)

    //グローバル変形行列の計算
    node.globalTransform = node.transform;
    if (parent)
    {
        node.parent = parent;
        //親の変形を乗算
        node.globalTransform *= parent->globalTransform;
    }

    //FBXノードのメッシュ情報を解析
    FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

    if (fbxNodeAttribute)
    {
        if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            model->meshNode = &node;
            ParseMesh(model, fbxNode);
        }
    }

    //子ノードに対して再帰呼び出し
    for (int i = 0; i < fbxNode->GetChildCount(); i++)
    {
        ParseNodeRecursive(model, fbxNode->GetChild(i));
    }
}

void FbxLoader::ParseMesh(Model* model, FbxNode* fbxNode)
{
    //ノードのメッシュを取得
    FbxMesh* fbxMesh = fbxNode->GetMesh();
    //頂点座標読み取り
    ParseMeshVertices(model, fbxMesh);
    //面を構成するデータの読み取り
    ParseMeshFaces(model, fbxMesh);
    //マテリアルの読み取り
    ParseMaterial(model, fbxNode);
    //スキニング情報の読み取り
    ParseSkin(model, fbxMesh);
}

void FbxLoader::ParseMeshVertices(Model* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;

    //頂点座標データの数
    const int controlPointCount =
        fbxMesh->GetControlPointsCount();
    //必要数だけ頂点データの配列を確保
    Model::VertexPosNormalUvSkin vert{};
    model->vertices.resize(controlPointCount,
        vert);

    //FBXメッシュの頂点座標配列を取得
    FbxVector4* pCoord =
        fbxMesh->GetControlPoints();

    //FBXメッシュの全頂点座標をモデル内の配列をコピーする
    for (int  i = 0; i < controlPointCount; i++)
    {
        Model::VertexPosNormalUvSkin& vertex =
            vertices[i];
        //座標をコピー
        vertex.pos.x = (float)pCoord[i][0];
        vertex.pos.y = (float)pCoord[i][1];
        vertex.pos.z = (float)pCoord[i][2];
    }

}

void FbxLoader::ParseMeshFaces(Model* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;
    auto& indices = model->indices;

    // 1ファイルに複数メッシュのモデルは非対応
    //assert(indices.size() == 0);
    //面の数
    const int polygonCount = fbxMesh->GetPolygonCount();
    //UVデータの数
    const int textureUVCount = fbxMesh->GetTextureUVCount();
    //UV名リスト
    FbxStringList uvNames;
    fbxMesh->GetUVSetNames(uvNames);

    for (int i = 0; i < polygonCount; i++)
    {
        //面を構成する頂点の数を取得(3なら三角形ポリゴン)
        const int polygonSize = fbxMesh->GetPolygonSize(i);
        //assert(polygonSize <= 4);

        //1頂点ずつ処理
        for (int j = 0; j < polygonSize; j++)
        {
            //FBX頂点配列のインデックス
            int index = fbxMesh->GetPolygonVertex(i, j);
            assert(index >= 0);

            //頂点法線読み取り
            Model::VertexPosNormalUvSkin& vertex = vertices[index];
            FbxVector4 normal;
            if (fbxMesh->GetPolygonVertexNormal(i,j,normal))
            {
                vertex.normal.x = (float)normal[0];
                vertex.normal.y = (float)normal[1];
                vertex.normal.z = (float)normal[2];
            }

            //テクスチャUV読み取り
            if (textureUVCount > 0)
            {
                FbxVector2 uvs;
                bool lUnmappedUV;

                //0番決め打ちで読込
                if (fbxMesh->GetPolygonVertexUV(i,j,
                    uvNames[0],uvs, lUnmappedUV))
                {
                    vertex.uv.x = (float)uvs[0];
                    vertex.uv.y = (float)uvs[1];
                }

                //インデックス配列に頂点インデックス追加
                //3頂点までなら
                if (j < 3)
                {
                    //1点追加し、他の2点と三角形を構成する
                    indices.push_back(index);
                }
                //4頂点目
                else
                {
                    //3点追加し
                    //四角形の0,1,2,3の内2,3,0で三角形を構成する
                    int index2 = indices[indices.size() - 1];
                    int index3 = index;
                    int index0 = indices[indices.size() - 3];
                    indices.push_back(index2);
                    indices.push_back(index3);
                    indices.push_back(index0);
                }
            }
        }
    }
}

void FbxLoader::ParseMaterial(Model* model, FbxNode* fbxNode)
{
    const int materialCount = fbxNode->GetMaterialCount();
    if (materialCount > 0)
    {
        //先頭のマテリアルを取得
        FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
        //テクスチャを読み込んだかどうかを表すフラグ
        bool textureLoaded = false;

        if (material)
        {
            //FBXSurfaceLambertクラス化どうかを調べる
            if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
            {
                FbxSurfaceLambert* lambert =
                    static_cast<FbxSurfaceLambert*>(material);

                //環境光係数
                FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
                model->ambient.x = (float)ambient.Get()[0];
                model->ambient.y = (float)ambient.Get()[1];
                model->ambient.z = (float)ambient.Get()[2];
            
                //拡散反射光係数
                FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
                model->diffuse.x = (float)diffuse.Get()[0];
                model->diffuse.y = (float)diffuse.Get()[1];
                model->diffuse.z = (float)diffuse.Get()[2];

                //ディフューズテクスチャを取り出す
                const FbxProperty diffuseProperty =
                    material->FindProperty(FbxSurfaceMaterial::sDiffuse);
                if (diffuseProperty.IsValid())
                {
                    const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
                    if (texture)
                    {
                        const char* filepath = texture->GetFileName();
                        //ファイルパスからファイル名抽出
                        string path_str(filepath);
                        string name = ExtractFileName(path_str);
                        //テクスチャ読み込み
                        LoadTexture(model, baseDirectory + model ->name + "/" + name);
                        textureLoaded = true;
                    }
                }
            }
        }

        //テクスチャがない場合白テクスチャを貼る
        if (!textureLoaded)
        {
            LoadTexture(model, baseDirectory +
                defaultTextureFileName);
        }
    }
}

void FbxLoader::LoadTexture(Model* model, const std::string& fullpath)
{
    HRESULT result = S_FALSE;
    //WFCテクスチャのロード
    TexMetadata& metadata = model->metadata;
    ScratchImage& scratchImg = model->scratchImg;
    //ユニコード文字列に変換
    wchar_t wfilepath[128];
    MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath,
        _countof(wfilepath));
    result = LoadFromWICFile(
        wfilepath, WIC_FLAGS_NONE,
        &metadata, scratchImg);
    if (FAILED(result))
    {
        assert(0);
    }
}

std::string FbxLoader::ExtractFileName(const std::string& path)
{
    size_t pos1;
    //区切り文字　\\ が出てくる一番最後の部分を検索
    pos1 = path.rfind("\\");
    if (pos1 != string::npos)
    {
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }
    //区切り文字 / が出てくる一番最後の部分を検索
    pos1 = path.rfind('/');
    if (pos1 != string::npos)
    {
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }

    return path;
}

void FbxLoader::ParseSkin(Model* model, FbxMesh* fbxMesh)
{
    //スキニング情報
    FbxSkin* fbxSkin =
        static_cast<FbxSkin*>(fbxMesh->GetDeformer(0,
            FbxDeformer::eSkin));
    //スキニング情報がなければ終了
    if (fbxSkin == nullptr)
    {
        //各頂点について処理
        for (int i = 0; i < model->vertices.size(); i++)
        {
            //最初のボーン(単位行列)の影響を100%にする
            model->vertices[i].boneIndex[0] = 0;
            model->vertices[i].boneWeight[0] = 1.0f;
        }

        return;
    }

    //ボーン配列の参照
    std::vector<Model::Bone>& bones = model->bones;

    //ボーンの数
    int clusterCount = fbxSkin->GetClusterCount();
    bones.reserve(clusterCount);

    //全てのボーンについて
    for (int i = 0; i < clusterCount; i++)
    {
        //FBXボーン情報
        FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

        //ボーン自体のノードの名前を取得
        const char* boneName = fbxCluster->GetLink()->GetName();

        //新しくボーンを追加し、追加したボーンの参照を得る
        bones.emplace_back(Model::Bone(boneName));
        Model::Bone& bone = bones.back();
        //自作ボーンとFBXのボーンを紐づける
        bone.fbxCluster = fbxCluster;

        //FBXから初期姿勢行列を取得する
        FbxAMatrix fbxMat;
        fbxCluster->GetTransformLinkMatrix(fbxMat);

        //XMatrix型に変換する
        XMMATRIX initialPose;
        ComvertMatrixFromFbx(&initialPose, fbxMat);

        //初期姿勢行列を得る
        bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
    }

    //ボーン番号とスキンウェイトのペア
    struct WeightSet
    {
        UINT index;
        float weight;
    };

    //二次元配列(ジャグ配列)
    //list::頂点が影響を受けるボーンの全リスト
    //vector:それを全頂点分
    std::vector<std::list<WeightSet>>
        WeightLists(model->vertices.size());

    //全てのボーンについて
    for (int i = 0; i < clusterCount; i++)
    {
        //FBXボーン情報
        FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
        //このボーンに影響を受ける頂点の数
        int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
        //このボーンに影響を受ける頂点の配列
        int* controlPointIndices = fbxCluster->GetControlPointIndices();
        double* constrolPointWeights = fbxCluster->GetControlPointWeights();

        //影響を受ける全頂点について
        for (int j = 0; j < controlPointIndicesCount; j++)
        {
            //頂点番号
            int vertIndex = controlPointIndices[j];
            //スキンウェイト
            float weight = (float)constrolPointWeights[j];
            //その頂点の影響を受けるボーンリストに、ボーンウェイトのペアを追加
            WeightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
        }
    }

    //頂点配列書き換え用の参照
    auto& vertices = model->vertices;
    //各頂点について処理
    for (int i = 0; i < vertices.size(); i++)
    {
        //頂点のウェイトから最も大きい4つを選択
        auto& WeightList = WeightLists[i];
        //大小比較用のラムダ式を指定して降順にソート
        WeightList.sort(
            [](auto const& lhs, auto const& rhs)
            {
                //左の要素の方が大きければtrue,そうでなければfalseを返す
                return lhs.weight > rhs.weight;
            });

        int weightArrayIndex = 0;
        //降順ノード済みのウェイトリストから
        for (auto& WeightSet : WeightList)
        {
            //頂点データに書き込み
            vertices[i].boneIndex[weightArrayIndex] = WeightSet.index;
            vertices[i].boneWeight[weightArrayIndex] = WeightSet.weight;
            //4つに達したら終了
            if (++weightArrayIndex >= Model::MAX_BONE_INDICES)
            {
                float weight = 0.0f;
                //2番目以降のウェイトを合計
                for (int j = 1; j < Model::MAX_BONE_INDICES; j++)
                {
                    weight += vertices[i].boneWeight[j];
                }
                //合計で1.0f(100%)になるように調整
                vertices[i].boneWeight[0] = 1.0f - weight;
                break;
            }
        }
    }
}

void FbxLoader::Finalize()
{
    //各種FBXインスタンスの破棄
    fbxImporter->Destroy();
    fbxManager->Destroy();
}
