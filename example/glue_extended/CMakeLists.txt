include(OmrAssert)

omr_assert(TEST NOT OMR_GC_EXTENDED_API)

# When the extended API is on, the GC provides the following pieces of glue.
# Without the OMR_GC_EXTENDED_API flag, we must implement this glue ourselves.

target_sources(omr_example_gc_glue INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentDelegate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MarkingDelegate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ObjectIterator.cpp
)

target_include_directories(omr_example_gc_glue INTERFACE
	./
)
