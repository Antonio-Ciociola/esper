file(GLOB_RECURSE stmlib_srcs "*.cc")

idf_component_register(
  SRCS ${stmlib_srcs}
  INCLUDE_DIRS ".."
  PRIV_REQUIRES driver
)
