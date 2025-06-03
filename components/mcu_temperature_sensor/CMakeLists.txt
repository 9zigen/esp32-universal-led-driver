set(SOURCES "mcu_temperature_sensor.c")
idf_component_register(SRCS ${SOURCES}
        INCLUDE_DIRS .
        REQUIRES driver)