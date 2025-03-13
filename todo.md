* [INPROGRESS] add runtime to editor sync
* [INPROGRESS] implement playmode

* tmp files for storing scene file when going into play mode
* make entity list and variant list coherent because modified/removed variants may cause issue 
* embed engine window

// variant list is watching variant folder for any changes. in cause of a change to variant entity list should be notified

* add remove variant from entity to runtime

* make sure an entity can have 1 instance of a variant type
* support double type
* Implement adding elements to array
* investigate enums

* implement other entity operations: new entity, add variant to entity, remove variant from entity, 
* implement a way to refer other entities (using ids)

-----------------------------------------

* [DONE] stop variants being duplicated on editor side
* [DONE] implement pause
* [DONE] Find a way to real time edit entities from the editor
* [DONE] implement stop engine
* [DONE] value should be string
* [DONE] implement kill button
* [DONE] add property parser and code generation tools
* [DONE] add macros for common variant code
* [DONE] lifetime methods:
* [DONE] on_init()
* [DONE] on_update()
* [DONE] on_play_start()
* [DONE] on_play_update()
* [DONE] implement raylib layer
* [DONE] entity_id should be uint64
* [DONE] Add generate button to editor for variant generations. just run the engine headless and kill it a few seconds later
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

