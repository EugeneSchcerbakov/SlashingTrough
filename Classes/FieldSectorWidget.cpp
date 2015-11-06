//
//  FieldSectorWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "FieldSectorWidget.h"
#include "GameInfo.h"

FieldSectorWidget* FieldSectorWidget::create(FieldSector::Ptr sector)
{
    FieldSectorWidget *widget = new FieldSectorWidget(sector);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

FieldSectorWidget::FieldSectorWidget(FieldSector::Ptr sector)
: _sector(sector)
{
}

FieldSectorWidget::~FieldSectorWidget()
{
}

bool FieldSectorWidget::init()
{
    if (!cocos2d::Sprite3D::init()) {
        return false;
    }
    
    float w = _sector->getWidth();
    float h = _sector->getHeight();
    
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texs;
    cocos2d::Mesh::IndexArray indices;
    
    float tilesX = (float)_sector->getSquaresNumByX();
    float tilesY = (float)_sector->getSquaresNumByY();
    
    positions.push_back(0.0f);positions.push_back(0.0f);positions.push_back(0.0f);
    positions.push_back(w);positions.push_back(0.0f);positions.push_back(0.0f);
    positions.push_back(w);positions.push_back(h);positions.push_back(0.0f);
    positions.push_back(0.0f);positions.push_back(h);positions.push_back(0.0f);
    
    texs.push_back(0.0f * tilesY);texs.push_back(0.0f * tilesY);
    texs.push_back(1.0f * tilesX);texs.push_back(0.0f * tilesY);
    texs.push_back(1.0f * tilesX);texs.push_back(1.0f * tilesY);
    texs.push_back(0.0f * tilesY);texs.push_back(1.0f * tilesY);
    
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    int perVertexSizeInFloat = 0;
    std::vector<float> vertices;
    std::vector<cocos2d::MeshVertexAttrib> attribs;
    cocos2d::MeshVertexAttrib att;
    att.size = 3;
    att.type = GL_FLOAT;
    att.attribSizeBytes = att.size * sizeof(float);
    
    if (positions.size())
    {
        perVertexSizeInFloat += 3;
        att.vertexAttrib = cocos2d::GLProgram::VERTEX_ATTRIB_POSITION;
        attribs.push_back(att);
    }
    if (normals.size())
    {
        perVertexSizeInFloat += 3;
        att.vertexAttrib = cocos2d::GLProgram::VERTEX_ATTRIB_NORMAL;
        attribs.push_back(att);
    }
    if (texs.size())
    {
        perVertexSizeInFloat += 2;
        att.vertexAttrib = cocos2d::GLProgram::VERTEX_ATTRIB_TEX_COORD;
        att.size = 2;
        att.attribSizeBytes = att.size * sizeof(float);
        attribs.push_back(att);
    }
    
    bool hasNormal = (normals.size() != 0);
    bool hasTexCoord = (texs.size() != 0);
    //position, normal, texCoordinate into _vertexs
    size_t vertexNum = positions.size() / 3;
    for(size_t i = 0; i < vertexNum; i++)
    {
        vertices.push_back(positions[i * 3]);
        vertices.push_back(positions[i * 3 + 1]);
        vertices.push_back(positions[i * 3 + 2]);
        
        if (hasNormal)
        {
            vertices.push_back(normals[i * 3]);
            vertices.push_back(normals[i * 3 + 1]);
            vertices.push_back(normals[i * 3 + 2]);
        }
        
        if (hasTexCoord)
        {
            vertices.push_back(texs[i * 2]);
            vertices.push_back(texs[i * 2 + 1]);
        }
    }
    
    cocos2d::NodeDatas nodedatas;
    cocos2d::MaterialDatas materialdats;
    cocos2d::MeshDatas meshdatas;
    
    cocos2d::MeshData *meshdata = new cocos2d::MeshData();
    meshdata->attribs = attribs;
    meshdata->vertex = vertices;
    meshdata->subMeshIndices.push_back(indices);
    meshdata->subMeshIds.push_back("");
    
    cocos2d::ModelData modeldata;
    modeldata.matrialId = "ground_tex";
    
    cocos2d::NodeData nodedata;
    nodedata.id = "ground";
    nodedata.transform.translate(0.0f, 0.0f, 0.0f);
    nodedata.modelNodeDatas.push_back(&modeldata);
    
    cocos2d::NTextureData texdata;
    texdata.id = "tex";
    texdata.type = cocos2d::NTextureData::Usage::Diffuse;
    texdata.filename = cocos2d::FileUtils::getInstance()->fullPathForFilename("gamefield/square_0.png");
    texdata.wrapS = GL_REPEAT;
    texdata.wrapT = GL_REPEAT;
    
    cocos2d::NMaterialData matdata;
    matdata.textures.push_back(texdata);
    matdata.id = "ground_tex";
    
    meshdatas.meshDatas.push_back(meshdata);
    nodedatas.nodes.push_back(&nodedata);
    materialdats.materials.push_back(matdata);
    
    initFrom(nodedatas, meshdatas, materialdats);
    
    return true;
}

FieldSector::Ptr FieldSectorWidget::getSector() const
{
    return _sector;
}

void FieldSectorWidget::createDebugGreed()
{
    // not implemented yet
}
