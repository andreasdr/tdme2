![LOGO](https://raw.githubusercontent.com/andreasdr/tdme2/master/resources/github/tdme2-logo.png)

- What is TDMETerrainEditor?
    - TDME2 based 3D terrain editor 

- What is already working
    - setting up terrain properties
        - key, value map for what ever reason
    - create terrain from 1m x 1m up to 4000m x 4000m
    - load and save terrains
    - use a brush to alter terrain with predefined or user generated textures
      - operations are add, subtract, flatten, delete, smooth
      - load a predefined or user generated texture as brush, whereas the rgb color space is used as strength base
      - set up the texture scale
      - set up the strength which will be multiplied with strength base derived from texture rgb color space
      - foliage gets updated after terrain height change
    - create water meshes with a auto fill like algorithm at grabbed brush terrain height
    - use a brush to add or remove foliage with predefined or user generated textures
      - operations are add, subtract #wip
      - load a predefined or user generated texture as brush, whereas the rgb color space is used as density base
      - set up the texture scale
      - set up the density which will be multiplied with density base derived from texture rgb color space

- Links
    - TDME2 Engine, see [README.md](./README.md)
    - TDME2 Model Editor, see [README-ModelEditor.md](./README-ModelEditor.md)
    - TDME2 Particle System Editor, see [README-ParticleSystemEditor.md](./README-ParticleSystemEditor.md)
    - TDME2 Terrain Editor, see [README-TerrainEditor.md](./README-TerrainEditor.md)
    - TDME2 Scene Editor, see [README-SceneEditor.md](./README-SceneEditor.md)
    - TDME2 How to build, see [README-BuildingHowTo.md](./README-BuildingHowTo.md)

- Credits
    - Dominik Hepp
    - Kolja Gumpert
    - others
