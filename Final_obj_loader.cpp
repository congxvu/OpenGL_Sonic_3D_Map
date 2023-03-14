/*
Author: Cong Vu
Class : ECE4122 - A
Last Date Modified : 12 / 06 / 2022

Description: Final_obj_loader cpp file

*/


//#include "common_header.h"
#include <stdio.h>
#include <stdlib.h>
#include "Final_obj_loader.h"
#include <string>
#include <vector>

using namespace std;

#pragma comment(lib, "assimp.lib")

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

CVertexBufferObject Final_obj_loader::vboModelData;
UINT Final_obj_loader::uiVAO;
vector<CTexture> Final_obj_loader::tTextures;

/*-----------------------------------------------

Name:	GetDirectoryPath

Params:	sFilePath - guess ^^

Result: Returns directory name only from filepath.

/*---------------------------------------------*/

string GetDirectoryPath(string sFilePath)
{
	// Get directory path
	string sDirectory = "";
	RFOR(i, ESZ(sFilePath)-1) if (sFilePath[i] == '\\' || sFilePath[i] == '/')
	{
		sDirectory = sFilePath.substr(0, i+1);
		break;
	}
	return sDirectory;
}

Final_obj_loader::Final_obj_loader()
{
	bLoaded = false;
}

/*-----------------------------------------------

Name:	loadFile

Params:	sFilePath - guess ^^

Result: Loads model using Assimp library.

/*---------------------------------------------*/

bool Final_obj_loader::loadFile(const std::string &filename)
{
    const char *sFilePath = filename.c_str();
	if(vboModelData.GetBufferID() == 0)
	{
		vboModelData.CreateVBO();
		tTextures.reserve(50);
	}
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( sFilePath, 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType);

	if(!scene)
	{
	       
		return false;
	}

	const int iVertexTotalSize = sizeof(aiVector3D)*2+sizeof(aiVector2D);
	
	int iTotalVertices = 0;

	FOR(i, scene->mNumMeshes)
	{
		aiMesh* mesh = scene->mMeshes[i];
		if (!mesh->HasTextureCoords(0))
		  {
		    continue;
		  }
		int iMeshFaces = mesh->mNumFaces;
		iMaterialIndices.push_back(mesh->mMaterialIndex);
		int iSizeBefore = vboModelData.GetCurrentSize();
		iMeshStartIndices.push_back(iSizeBefore/iVertexTotalSize);
		FOR(j, iMeshFaces)
		{
			const aiFace& face = mesh->mFaces[j];
			FOR(k, 3)
			{
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
				vboModelData.AddData(&pos, sizeof(aiVector3D));
				vboModelData.AddData(&uv, sizeof(aiVector2D));
				vboModelData.AddData(&normal, sizeof(aiVector3D));
			}
		}
		int iMeshVertices = mesh->mNumVertices;
		iTotalVertices += iMeshVertices;
		iMeshSizes.push_back((vboModelData.GetCurrentSize()-iSizeBefore)/iVertexTotalSize);
	}
	iNumMaterials = scene->mNumMaterials;

	vector<int> materialRemap(iNumMaterials);

	FOR(i, iNumMaterials)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int a = 5;
		int texIndex = 0;
		aiString path;  // filename

		if(material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
		{
			string sDir = GetDirectoryPath(sFilePath);
			string sTextureName = path.data;
			string sFullPath = sDir+sTextureName;
			int iTexFound = -1;
			FOR(j, ESZ(tTextures))if(sFullPath == tTextures[j].GetPath())
			{
				iTexFound = j;
				break;
			}
			if(iTexFound != -1)materialRemap[i] = iTexFound;
			else
			{
				CTexture tNew;
				tNew.LoadTexture2D(sFullPath, true);
				materialRemap[i] = ESZ(tTextures);
				tTextures.push_back(tNew);
			}
		}
	}

	FOR(i, ESZ(iMeshSizes))
	{
		int iOldIndex = iMaterialIndices[i];
		iMaterialIndices[i] = materialRemap[iOldIndex];
	}

	return bLoaded = true;
}

/*-----------------------------------------------

Name:	FinalizeVBO

Params: none

Result: Uploads all loaded model data in one global
		models' VBO.

/*---------------------------------------------*/

void Final_obj_loader::FinalizeVBO()
{
	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);
	vboModelData.BindVBO();
	vboModelData.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), (void*)sizeof(aiVector3D));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), (void*)(sizeof(aiVector3D)+sizeof(aiVector2D)));
}

/*-----------------------------------------------

Name:	BindModelsVAO

Params: none

Result: Binds VAO of models with their VBO.

/*---------------------------------------------*/

void Final_obj_loader::BindModelsVAO()
{
	glBindVertexArray(uiVAO);
}

/*-----------------------------------------------

Name:	renderMeshes

Params: none

Result: Guess what it does ^^.

/*---------------------------------------------*/

void Final_obj_loader::renderMeshes()
{
	if(!bLoaded)return;
	int iNumMeshes = ESZ(iMeshSizes);
	FOR(i, iNumMeshes)
	{
		int iMatIndex = iMaterialIndices[i];
		tTextures[iMatIndex].BindTexture();
		glDrawArrays(GL_TRIANGLES, iMeshStartIndices[i], iMeshSizes[i]);
	}
}
