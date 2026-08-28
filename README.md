

## Install stb_image 
sudo apt install libstb-dev

## Importing Synty assets:
Step 1: Import Synty FBX into Blender
- Go to File > Import > FBX.
- In the right-hand import panel under Armature, check Ignore Leaf Bones and Force Connect Children (leave animations unchecked if you just want the rest pose).
- Select your imported model/armature, switch to Pose Mode, select all bones, and choose Pose > Clear Transform > All to fix initial distortion.
- Return to Object Mode, select all mesh parts and the armature, press Ctrl + A, and select All Transforms to zero out scale/rotation. Reorient the model upright if necessary (rotation X: 90 degrees) and apply transforms again.

Step 2: Fix Materials and Textures
- Synty assets often map to a single shared texture atlas (e.g., polygon texture maps)
  Switch your viewport shading to Material Preview or Rendered.
- If textures appear missing, open the Shader Editor, reconnect the Base Color of the Principled BSDF node to the image texture provided in the Synty asset folder.

Step 3: Export for Assimp / C++ / OpenGL
- Select your processed mesh and armature (if animated).
- Go to File > Export > FBX (or gltf 2.0).
- In FBX export settings, set Path Mode to Copy and check Embed Textures if you want self-contained asset packaging, or leave textures as external files alongside your model. Ensure Scale is set appropriately (usually FBX Unit Scale).

(Using gltf at this point in time, not using embedded texture)

### Fix the Asset Export in Blender
If you would prefer the engine code to remain untouched, you must fix the mesh topology inside Blender so that the glTF exporter writes out shared normals smoothly.

Open the Synty asset inside Blender.

Tab into Edit Mode, press A to select all vertices of the mesh.

Press M and choose Merge -> By Distance. (Synty meshes have disconnected overlapping vertices; this welds them together, which allows smooth gradients to flow across the edge fields).

Right-click the object in Object Mode and select Shade Smooth.Go to File -> Export -> glTF 2.0.

Under the Mesh drop-down on the export settings sidebar, make sure Normals is checked, and export.