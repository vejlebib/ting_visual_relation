api = 2
core = 7.x

; Contrib modules

projects[features][subdir] = "contrib"
projects[features][version] = "2.0"

projects[virtual_field][subdir] = "contrib"
projects[virtual_field][version] = "1.1"

; Libraries

libraries[d3][download][type] = "get"
libraries[d3][download][url] = "https://github.com/mbostock/d3/zipball/master"
libraries[d3][directory_name] = "d3"
libraries[d3][destination] = "libraries"
