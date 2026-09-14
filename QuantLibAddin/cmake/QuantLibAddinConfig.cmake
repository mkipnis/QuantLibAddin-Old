PACKAGE_INIT@

include(CMakeFindDependencyMacro)

find_dependency(Boost_REQUIED COMPONENTS serialization filesystem)
find_dependency(QuantLib_REQUIRED)
find_dependency(gensrc_REQUIRED)
find_dependency(ObjectHandler_REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/QuantLibAddinTargets.cmake")

check_required_components(QuantLibAddin)

