{
  "targets": [
    {
      "target_name": "screen_capture",
      "sources": ["screen_capture.cc"],
      "include_dirs": [
        "./node_modules/node-addon-api"
      ],
      "defines": [
        "NODE_ADDON_API_CPP_EXCEPTIONS"
      ],
      "libraries": [
        "dxgi.lib",
        "d3d11.lib",
        "ole32.lib",
        "uuid.lib"
      ]
    }
  ]
}