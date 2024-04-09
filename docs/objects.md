# Objects

Objects in this engine are represented mainly by scripts.

Objects communicate with the engine by sending and recieving smalltalk-style *messages*.

For example, the engine might send `(void) tick: (float) time` (or `(void) updatePosition: (float) time`) to indicate that an object should update its state. Or for another example, the engine might send the message `(Dictionary) pack` to indicate that an object should pack up its state into a dictionary for saving over the network.

Additionally, some methods may be pure "getter" methods, for example `(int) getAawakeFrameCount` or `(Shape) getDrawShape`.

The engine should never directly access an object's state.

Since the objects must exist somewhere, we keep them in the Universe. The Universe contains all objects in a flat array for storage and is effectively the global state of the engine.

The Universe is queryable using object attributes like IDs, given names (like HTML IDs), tags (like HTML classes), class, and maybe other attributes.

The Universe can also be accessed from objects themselves.

To implement something like scenes objects can be members of a Collection. 


## Example

### `assets://Main.tr`

```smalltalk
"Prototypes are used instead of classes; instance and class variables are implicit"
"This will be run implicity:"
"Main := Object sub."

"Smalltalk-T blocks have selectors"
Main method: [init |
	| x y |
	
	x := 8.
	y := 'GameWorld'.
	
	"Presumably you'd not have to create a universe in the actual engine."
	universe := Universe new.
	universe prealloc: x.
	
	gameWorld := Collection newWithName: y forUniverse: universe.
	gameWorld setAutoInclusionPredicate: [:object | ^(object tags) contains: '_Level'.].
]

Main method: [setPaused: paused forCollectionNamed: collection |
	"..."
]
```

```lisp
(defun init ()
	(defvar *universe* (universe-new))
	(let ( (x 8) (y "GameWorld") )
		(universe-prealloc x)
		(collection-new y *universe*)
		(collection-set-auto-include-predicate (lambda (x) (member "_Level" x)))
	)
)
(defun set-paused (paused collection)
	; ...
)
```
