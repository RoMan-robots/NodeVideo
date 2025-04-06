{
  "targets": [
    {
      "target_name": "screen_capture",
      "sources": ["screen_capture.cc"],
      "cflags_cc": ["/DNODE_ADDON_API_CPP_EXCEPTIONS"],
      "include_dirs": [
        "./node_modules/node-addon-api"
      ]
    }
  ]
}