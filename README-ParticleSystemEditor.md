![LOGO](https://raw.githubusercontent.com/andreasdr/tdme2/master/resources/logos/github-logo.png)

- What is TDMEParticleSystemEditor?
    - TDME based particle system editor 

- What is already working
    - particle system loading
    - particle system saving
    - render with generated ground, shadowing, bounding volume
    - rotate, zoom
    - setting up particle system properties
        - key, value map for what ever reason
    - setting up a particle system group which consists of multiple particle systems, for each you can
      - set up the type
          - object particle system
          - point particle system
          - fog particle system
      - set up and edit the emitter
          - point particle emitter
          - bounding box particle emitter
          - circle particle emitter
          - circle particle emitter with velocity on circle plane
          - sphere particle emitter
      -  emitters can be transformed by using a gizmo
    - setting up bounding volumes
        - none
        - sphere
        - capsule
        - bounding box
        - oriented bounding box
        - convex mesh
        - bounding volumes can be transformed by using a gizmo 
    - sound
        - attach sounds to optional animations with optional additional key
        - also set up gain, pitch, looping, fixed and offset for each sound

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
