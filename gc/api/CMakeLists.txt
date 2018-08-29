include(OmrAssert)

omr_assert(TEST OMR_GC_EXTENDED_API)

target_sources(omrgc
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/OMR_GC_System.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/MarkingDelegate.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/glue/EnvironmentDelegate.cpp
)

target_include_directories(omrgc
	PUBLIC
		include/
		glue/
)

# https://cmake.org/cmake/help/v3.6/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html#prop_gbl:CMAKE_CXX_KNOWN_FEATURES
target_compile_features(omrgc
	PUBLIC
		cxx_constexpr
		# cxx_auto_type
		cxx_deleted_functions
		cxx_delegating_constructors
		cxx_noexcept
		cxx_nullptr
		cxx_override
		cxx_right_angle_brackets
		cxx_variadic_templates
		cxx_template_template_parameters
)
