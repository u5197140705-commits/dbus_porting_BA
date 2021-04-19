{
    "abstract":
    {   // TODO: select a appropriate description
        "description": "Firmware for variant_cm4_stm32l4",
        "uid": UID,
        "version":
        {
            "major": VERSION_MAJOR,
            "minor": VERSION_MINOR,
            "revision": VERSION_REVISION,
            "build": VERSION_BUILD,
        },
        "name": "MyVariant SW",
        "targetNode": DBUS_TARGET_NODE,
        "techReleaseNotes": RELEASE_NOTE,
        "installType": "INSTALLED_BY_CENTRAL_MODULE",
        "firmwareComponentType": "Firmware",
        "firmwareType": "Base_Software"
    },
    "packaging_info":
    {
        "swupdate":
        {
            "files":
            [
                {   // TODO: edit path and filename (same than partition name!)
                    "filename": "Application",
                    "path": "/mnt/dbus2-nodes/CPM/Application",
                    "installed-directly": true,
                    "properties":
                    {
                        "create-destination": "true"
                    }
                }
                #ifdef FWU_APP1_START_ADDRESS2
                ,{
                    "filename": "App_part2",
                    "path": "/mnt/dbus2-nodes/CPM/App_part2",
                    "installed-directly": true,
                    "properties":
                    {
                        "create-destination": "true"
                    }
                }
                #endif
                // TODO: add additional memory modules here
            ]
        }
    }
}