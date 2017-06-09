{
  "targets": [
      {
          "target_name": "main",
          "sources": [
              "addons/python.cc"
          ],
          'libraries': [
              '<!@(pkg-config python2 --libs)',
          ],
          'include_dirs': [
              '<!@(pkg-config python2 --cflags-only-I | sed s/-I//g)'
          ]
      }
  ]
}