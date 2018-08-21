#if !defined(OMR_OM_JITHELPERS_HPP_)
#define OMR_OM_JITHELPERS_HPP_

#include <OMR/Om/Object.hpp>
#include <OMR/Om/Context.hpp>

extern "C" {

OMR::Om::Object* omr_om_allocate_empty_object(Context* cx);

OMR::Om::Object* omr_om_allocate_instance(Context* cx, Shape* shape);

} // extern "C"

#endif // OMR_OM_JITHELPERS_HPP_
