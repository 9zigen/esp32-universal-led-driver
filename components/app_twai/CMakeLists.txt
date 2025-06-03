set(SOURCES "app_twai.c")
idf_component_register(SRCS ${SOURCES}
        INCLUDE_DIRS . include
        REQUIRES driver app_settings)