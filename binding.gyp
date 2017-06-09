{
  'targets': [
      {
          'target_name': 'pyvm',
          'sources': [
              "addons/convert.cc",
              "addons/pyvm.cc"
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
