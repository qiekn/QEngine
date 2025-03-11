* implement .scene file
* tmp files for storing scene file when going into play mode

lifetime methods:

on_init()
on_update()
on_play_start()
on_play_update()


* add playmode
* add remove variant from entity to runtime
* add runtime to editor sync

* Add generate button to editor for variant generations. just run the engine headless and kill it a few seconds later
* Find a way to real time edit entities from the editor-engine and reflect to each other

* make sure an entity can have 1 instance of a variant type
* support double type
* Implement adding elements to array
* investigate enums


* value should be string
* entity_id should be uint64

* implement other entity operations: new entity, add variant to entity, remove variant from entity, 

* implement playmode
* implement stop engine


-----------------------------------------

* implement a way to refer other entities (using ids)
* implement raylib layer


* [DONE] implement component ref, entity ref
* [DONE] make deserialize entity return entity id 
* [DONE] Implemenet a long entity for sample
* [DONE] implement dummy type files for editor
* [DONE] remove component class, use variant instead
* [DONE] change folder names
* [DONE] implemenet .entity file serialization/deserialization. entity has an id and list of variants
* [DONE] improve api
* [DONE] add update methods to variants ()
* [DONE] Start editor layer
