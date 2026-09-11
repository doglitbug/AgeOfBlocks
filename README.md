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
    - Check Ignore Leaf Bones (this removes unneeded end bones)
    - Check Force Connect Children (this makes the bones align properly instead of facing out at 90 angles)
- Click 'Import FBX'
- Delete unwanted skins(meshes) (e.g. just keep SM_Prince_01)

Step 2: Fix Materials and Textures
- Synty assets often map to a single shared texture atlas (e.g., polygon texture maps)
  Switch your viewport shading to Material Preview or Rendered.
- If textures appear missing, open the Shader Editor, reconnect the Base Color of the Principled BSDF node to the image texture provided in the Synty asset folder.

Step 3: Export
- Go to File > Export > gltf 2.0.
  - Under Format, select glTF Separate
  - Under Data > Mesh, untick Normals (this smoothes the faces instead of them being a solid color)


## Aligning Kevin Iglesias animations

This process is to align the Synty bones with the KI animations

[Retarget Animations to Any Rig in Blender](https://www.youtube.com/watch?v=Op0AvUChNTo)

[Retargeting Animations Using Blender Rokoko Plugin](https://www.youtube.com/watch?v=6kj_ZX_lIL4)

Step 1: Import Animation
- Go to File > Import > FBX (Legacy)
- Under Transform, set Scale to 0.01
- Check Animation
- Check Armature > Automatic Bone Orientation (?? is this needed??)

Step 2 ???
Step 3 ???

 From Almost.blend:
 
Change Synty Armature from X rotation 90 to 180
In Rokoko select rest and click retarget. Should be aligned correctly.


Change synty Armature back to 90 before exporting skin (Due to z-up/y-up)