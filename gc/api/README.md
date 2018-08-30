# OMR GC Extended APIs

The extended APIs provide a higher level of abstraction over the GC, and come complete with a more inclusive set of features than the base GC.

As a result, the glue interfaces change slightly, and in some cases don't need to be implemented at all.

This component extends the OMR GC with the following utility APIs:

1. Startup and shutdown wrappers
2. A built-in rooting API
3. Heap access semantics, and a shared lock.


## Implementing the Glue

### Object Description

`OMRClient/GC/ObjectDescription.hpp`

```c++
#if !defined(OMRCLIENT_GC_OBJECTDESCRIPTION_HPP_)
#define OMRCLIENT_GC_OBJECTDESCRIPTION_HPP_

namespace OMRClient {
namespace GC {

struct ObjectTraits {
	using PointerType = void*;
};

}  // namespace GC
}  // namespace OMRClient

#endif // OMRCLIENT_GC_OBJECTDESCRIPTION_HPP_
```

### Object Scanning

`OMRClient/GC/ObjectScanner.hpp`

```c++
#if !defined(OMRCLIENT_GC_OBJECTSCANNER_HPP_)
#define OMRCLIENT_GC_OBJECTSCANNER_HPP_

#include <OMR/GC/RefSlotHandle.hpp>

namespace OMRClient {
namespace GC {

class ObjectScanner {
public:
	template <typename T>
	bool scan(VisitorT visitor, ObjectTraits::PointerType object) {
		// TODO
		return false;
	}

	template <typename VisitorT>
	bool scan(VisitorT visitor) {
		return false;
	}
};

}  // namespace GC
}  // namespace OMRClient

#endif // OMRCLIENT_GC_OBJECTSCANNER_HPP_

```

## Startup and Shutdown

```c++
OMR::Runtime runtime;
OMR::GC::System system(runtime);
OMR::GC::Context cx(system);
```

## Allocating
### Byte Allocators

```c++
void* ref = OMR::GC::allocate(cx, nbytes);
```

### Building bigger allocators

```c++
allocateObject()
```