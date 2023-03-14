/*
Author: Cong Vu
Class : ECE4122 - A
Last Date Modified : 12 / 06 / 2022

Description: Final_obj_loader cpp file

*/

#pragma once

#include "common/shader.hpp"
#include "vertexBufferObject.h"
#include "texture.h"

class CMaterial
{
public:
	int iTexture;
};

class Final_obj_loader
{
public:
	bool loadFile(const std::string &filename);

	static void FinalizeVBO();
	static void BindModelsVAO();

	void renderMeshes();
	Final_obj_loader();
private:
	bool bLoaded;
	static CVertexBufferObject vboModelData;
	static UINT uiVAO;
	static vector<CTexture> tTextures;
	vector<int> iMeshStartIndices;
	vector<int> iMeshSizes;
	vector<int> iMaterialIndices;
	int iNumMaterials;
};
