# Steps for development

## Required libraries
* Install stb_image 
```sudo apt install libstb-dev```

## Importing Synty assets:
Step 1: Import Synty FBX into Blender (5.2.0.LTS)
- Go to File > Import > FBX (Legacy)
- In the right-hand import panel:
  - Uncheck Animation
  - Under Armature:
    - Check Ignore Leaf Bones
    - Check Force Connect Children
- Click 'Import FBX' 
- Delete unwanted skins (eg just keep SM_Prince_01)

Step 2: Fix Materials and Textures
- Synty assets often map to a single shared texture atlas (e.g., polygon texture maps)
  Switch your viewport shading to Material Preview or Rendered.
- If textures appear missing, open the Shader Editor, reconnect the Base Color of the Principled BSDF node to the image texture provided in the Synty asset folder.

Step 3: Export
- Go to File > Export > gltf 2.0.
  - Under Format, select glTF Separate
  - Under Mesh, untick Normals