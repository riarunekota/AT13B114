//=====================================
//    
//   debugcollision.cpp
//              created by Koki Nakano
//
//=====================================


// <説明>
// これはデバッグ用、当たり判定描画のソースコードです。
// 村瀬クラスで配布されたrenderer.hが必要になります。
// static変数なのでインスタンスを生成せずに関数を書くことができます。
// SphereとCubeは別モノなので両方書いてください
// それと、付属のwhite.pngもアセットフォルダに入れといてください。

//法線の関係上回転をすると裏側が黒くなるので、自分で設定したライトを切ってください。
//ガイドをDraw関数の中にコメントで書いています。



#include "main.h"
#include "renderer.h"
#include "debugcollision.h"



//画像のファイルパス
#define DEBUGCOLLISION_TEXTURE_PASS "asset/texture/white.png"





//Sphere用
ID3D11ShaderResourceView* DebugSphereCollision::m_Texture;        //テクスチャ
ID3D11Buffer* DebugSphereCollision::m_VertexBuffer[3];            //頂点バッファ

VERTEX_3D DebugSphereCollision::circlevertexZX[CIRCLEVERTEX];
VERTEX_3D DebugSphereCollision::circlevertexXY[CIRCLEVERTEX];
VERTEX_3D DebugSphereCollision::circlevertexYZ[CIRCLEVERTEX];


//Cube用
ID3D11ShaderResourceView* DebugCubeCollision::m_Texture;          //テクスチャ
ID3D11Buffer*             DebugCubeCollision::m_VertexBuffer;     //頂点バッファ




//-----------------------------------
//  Spehre
//-----------------------------------
void DebugSphereCollision::Init()
{
	float one_deg = 360.0f / CIRCLEVERTEX;     //1頂点ごとの角度


	//XY円
	for (int i = 0; i < CIRCLEVERTEX; i++)
	{
		circlevertexXY[i].Position = D3DXVECTOR3(cosf(D3DXToRadian(i * (360.0f / (CIRCLEVERTEX - 1)))), sinf(D3DXToRadian(i * (360.0f / (CIRCLEVERTEX - 1)))), 0.0f);
		circlevertexXY[i].Diffuse  = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		circlevertexXY[i].Normal   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		circlevertexXY[i].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	}

	//YZ円
	for (int i = 0; i < CIRCLEVERTEX; i++)
	{
		circlevertexYZ[i].Position = D3DXVECTOR3(0.0f, cosf(D3DXToRadian(i * (360.0f / (CIRCLEVERTEX - 1)))), sinf(D3DXToRadian(i * (360.0f / (CIRCLEVERTEX - 1)))));
		circlevertexYZ[i].Diffuse  = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		circlevertexYZ[i].Normal   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		circlevertexYZ[i].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	}

	//ZX円
	for (int i = 0; i < CIRCLEVERTEX; i++)
	{
		circlevertexZX[i].Position = D3DXVECTOR3(cosf(D3DXToRadian(i * (360.0f / (CIRCLEVERTEX - 1)))), 0.0f, sinf(D3DXToRadian(i * (360.0f / (CIRCLEVERTEX - 1)))));
		circlevertexZX[i].Diffuse  = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		circlevertexZX[i].Normal   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		circlevertexZX[i].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	}



	//頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));                            //構造体の中身を0で埋める
	bd.Usage     = D3D11_USAGE_DYNAMIC;                     //頂点の情報を後で書き換えられるようにする
	bd.ByteWidth = sizeof(VERTEX_3D) * CIRCLEVERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;                //頂点バッファ
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;             //頂点の情報を後で書き換えられるようにする

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));


	//XY円
	sd.pSysMem = circlevertexXY;
	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer[0]);     //頂点バッファやインデックスバッファなど

	//YX円
	sd.pSysMem = circlevertexYZ;
	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer[1]);     //頂点バッファやインデックスバッファなど

	//ZX円
	sd.pSysMem = circlevertexZX;
	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer[2]);     //頂点バッファやインデックスバッファなど
	
																			

	//テクスチャ読み込み
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		DEBUGCOLLISION_TEXTURE_PASS,
		NULL,
		NULL,
		&m_Texture,
		NULL);

	//読み込み確認
	assert(m_Texture);
}



void DebugSphereCollision::Uninit()
{
	m_VertexBuffer[0]->Release();
	m_VertexBuffer[1]->Release();
	m_VertexBuffer[2]->Release();

	m_Texture->Release();
}



void DebugSphereCollision::Draw(D3DXVECTOR3 position, D3DXVECTOR3 rotation, float radius, D3DXCOLOR color)
{
#if defined(_DEBUG) || defined(DEBUG)
	////ライト無効化
	//LIGHT light;
	//light.Enable = false;
	//light.Direction = D3DXVECTOR4(1.0f, -1.0f, 1.0f, 0.0f);  //方向
	//D3DXVec4Normalize(&light.Direction, &light.Direction);   //ベクトルを1にする
	//light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.2f);       //環境光
	//light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);       //直接光
	//CRenderer::SetLight(light);


	//マトリクス設定
	D3DXMATRIX mtxW, mtxT, mtxS, mtxR;
	D3DXMatrixScaling(&mtxS, radius, radius, radius);
	D3DXMatrixRotationYawPitchRoll(&mtxR, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&mtxT, position.x, position.y, position.z);
	mtxW = mtxS * mtxR * mtxT;
	CRenderer::SetWorldMatrix(&mtxW);


	//マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = color;
	CRenderer::SetMaterial(material);

	//テクスチャ設定
	CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);


	//円の描画
	Draw_XYCircle();
	Draw_YZCircle();
	Draw_ZXCircle();


	////ライト無効化
	//light.Enable = true;
	//CRenderer::SetLight(light);
#endif
}




void DebugSphereCollision::Draw(D3DXVECTOR3 position, D3DXQUATERNION quaternion, float radius, D3DXCOLOR color)
{
#if defined(_DEBUG) || defined(DEBUG)
	////ライト無効化
	//LIGHT light;
	//light.Enable = false;
	//light.Direction = D3DXVECTOR4(1.0f, -1.0f, 1.0f, 0.0f);  //方向
	//D3DXVec4Normalize(&light.Direction, &light.Direction);   //ベクトルを1にする
	//light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.2f);       //環境光
	//light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);       //直接光
	//CRenderer::SetLight(light);



	//マトリクス設定
	D3DXMATRIX mtxW, mtxT, mtxS, mtxR;
	D3DXMatrixScaling(&mtxS, radius, radius, radius);
	D3DXMatrixRotationQuaternion(&mtxR, &quaternion);
	D3DXMatrixTranslation(&mtxT, position.x, position.y, position.z);
	mtxW = mtxS * mtxR * mtxT;

	CRenderer::SetWorldMatrix(&mtxW);



	//マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = color;
	CRenderer::SetMaterial(material);

	//テクスチャ設定
	CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);



	Draw_XYCircle();
	Draw_YZCircle();
	Draw_ZXCircle();


	////ライト無効化
	//light.Enable = true;
	//CRenderer::SetLight(light);
#endif
}





void DebugSphereCollision::Draw_XYCircle()
{
	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[0], &stride, &offset);


	
	//ポリゴン描画(頂点数,)
	CRenderer::GetDeviceContext()->Draw(CIRCLEVERTEX, 0);
}


void DebugSphereCollision::Draw_YZCircle()
{
	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[1], &stride, &offset);



	//ポリゴン描画(頂点数,)
	CRenderer::GetDeviceContext()->Draw(CIRCLEVERTEX, 0);
}


void DebugSphereCollision::Draw_ZXCircle()
{
	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[2], &stride, &offset);



	//ポリゴン描画(頂点数,)
	CRenderer::GetDeviceContext()->Draw(CIRCLEVERTEX, 0);
}












void DebugCubeCollision::Init()
{
	VERTEX_3D vertex[24];

	//座標の設定
	{
		//上
		vertex[0].Position  = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		vertex[1].Position  = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		vertex[2].Position  = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
		vertex[3].Position  = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
				  		    
		vertex[4].Position  = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		vertex[5].Position  = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
		vertex[6].Position  = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		vertex[7].Position  = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
			
		//下
		vertex[8].Position  = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		vertex[9].Position  = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		vertex[10].Position = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		vertex[11].Position = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);

		vertex[12].Position = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		vertex[13].Position = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		vertex[14].Position = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		vertex[15].Position = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
				   
		//縦
		vertex[16].Position = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
		vertex[17].Position = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
		vertex[18].Position = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
		vertex[19].Position = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
				   
		vertex[20].Position = D3DXVECTOR3( 0.5f, -0.5f, -0.5f);
		vertex[21].Position = D3DXVECTOR3( 0.5f,  0.5f, -0.5f);
		vertex[22].Position = D3DXVECTOR3( 0.5f, -0.5f,  0.5f);
		vertex[23].Position = D3DXVECTOR3( 0.5f,  0.5f,  0.5f);
	}

	//そのほかの設定
	for (int i = 0; i < 24; i++)
	{
		vertex[i].Diffuse  = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[i].Normal   = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vertex[i].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	}



	//頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));                            //構造体の中身を0で埋める
	bd.Usage = D3D11_USAGE_DYNAMIC;                     //頂点の情報を後で書き換えられるようにする
	bd.ByteWidth = sizeof(VERTEX_3D) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;                //頂点バッファ
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;             //頂点の情報を後で書き換えられるようにする

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;
	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);     //頂点バッファやインデックスバッファなど



	//テクスチャ読み込み
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		DEBUGCOLLISION_TEXTURE_PASS,
		NULL,
		NULL,
		&m_Texture,
		NULL);

	//読み込み確認
	assert(m_Texture);
}



void DebugCubeCollision::Uninit()
{
	m_VertexBuffer->Release();
	m_Texture->Release();
}



void DebugCubeCollision::Draw(D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 radius, D3DXCOLOR color)
{
#if defined(_DEBUG) || defined(DEBUG)
	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);


	//マトリクス設定
	D3DXMATRIX mtxW, mtxT, mtxS, mtxR;
	D3DXMatrixScaling(&mtxS, radius.x, radius.y, radius.z);
	D3DXMatrixRotationYawPitchRoll(&mtxR, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&mtxT, position.x, position.y, position.z);
	mtxW = mtxS * mtxR * mtxT;

	CRenderer::SetWorldMatrix(&mtxW);



	//マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = color;
	CRenderer::SetMaterial(material);

	//テクスチャ設定
	CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


	//ポリゴン描画(頂点数,)
	CRenderer::GetDeviceContext()->Draw(CIRCLEVERTEX, 0);
#endif
}



void DebugCubeCollision::Draw(D3DXVECTOR3 position, D3DXQUATERNION quaternion, D3DXVECTOR3 radius, D3DXCOLOR color)
{
#if defined(_DEBUG) || defined(DEBUG)
	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);


	//マトリクス設定
	D3DXMATRIX mtxW, mtxT, mtxS, mtxR;
	D3DXMatrixScaling(&mtxS, radius.x, radius.y, radius.z);
	D3DXMatrixRotationQuaternion(&mtxR, &quaternion);
	D3DXMatrixTranslation(&mtxT, position.x, position.y, position.z);
	mtxW = mtxS * mtxR * mtxT;

	CRenderer::SetWorldMatrix(&mtxW);


	//マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = color;
	CRenderer::SetMaterial(material);


	//テクスチャ設定
	CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//ポリゴン描画(頂点数,)
	CRenderer::GetDeviceContext()->Draw(24, 0);
#endif
}