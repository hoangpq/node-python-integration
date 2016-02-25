{
  "targets": [
    {
      "target_name": "python",
      "sources": ["addons/python.cc"],
      "include_dirs": [
        "C:\Python27\include",
        "C:\Ruby200-x64\include\ruby-2.0.0",
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [
        "C:\Python27\libs\python27.lib"
      ]
    }
  ]
}