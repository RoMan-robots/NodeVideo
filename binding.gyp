{
  "targets": [
    {
      "target_name": "screen_capture",
      "sources": ["screen_capture.cc"],
      "include_dirs": [
        "<!(node -e \"require('node-addon-api')\")"
      ]
    }
  ]
}