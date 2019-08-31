﻿// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,28.08.2019</created>
/// <changed>ʆϒʅ,01.09.2019</changed>
// ********************************************************************************

#ifndef TEXTURE_H
#define TEXTURE_H


#include <d3d10_1.h>


// targa texture file type
struct TargaHeader
{
  unsigned char dataOne [12];
  unsigned short width;
  unsigned short height;
  unsigned char bpp; // bit per pixel
  unsigned short dataTwo;
};


// png texture file typev
struct PngHeader {};


// operations on textures wrapper
template <typename fileType>
class Texture
{
private:
  fileType file; // different file formats
  unsigned char* data; // raw file data holder
  ID3D10Texture2D* texture; // structured texture data, rendered by DirectX
  ID3D10ShaderResourceView* textureView; // resource view, holder of texture data drawn by shader

  bool Load ( const char* ); // specialized defined file loader
public:
  Texture ( ID3D10Device1*, const char* );
  ID3D10ShaderResourceView** const getTexture ( void ); // get texture data (unsuccessful texture file loading: nullptr)
  void Release ( void ); // resource releaser
};
void O2DmodelClassLinker ( void ); // don't call this function: solution for linker error, when using templates.


#endif // !TEXTURE_H