idf_component_register(
    SRCS
        "epd_demo.c"  # main app
        "epd.c"       # display driver
        "epd_esp32.c" # adaptation of driver to esp32
    INCLUDE_DIRS
        "."
)
