api = 2
core = 7.x

; Contrib modules

projects[features][subdir] = "contrib"
projects[features][version] = "2.0"

projects[virtual_field][subdir] = "contrib"
projects[virtual_field][version] = "1.1"

; Ding2 modules

projects[ting][type] = "module"
projects[ting][download][type] = "git"
projects[ting][download][url] = "git@github.com:ding2tal/ting.git"
projects[ting][download][tag] = "7.x-1.0"

projects[ting_covers][type] = "module"
projects[ting_covers][download][type] = "git"
projects[ting_covers][download][url] = "git@github.com:ding2tal/ting_covers.git"
projects[ting_covers][download][tag] = "7.x-1.0"

projects[ting_search][type] = "module"
projects[ting_search][download][type] = "git"
projects[ting_search][download][url] = "git@github.com:ding2tal/ting_search.git"
projects[ting_search][download][tag] = "7.x-1.0"

projects[ting_fulltext][type] = "module"
projects[ting_fulltext][download][type] = "git"
projects[ting_fulltext][download][url] = "git@github.com:ding2tal/ting_fulltext.git"
projects[ting_fulltext][download][tag] = "7.x-1.0"

projects[ding_ting_frontend][type] = "module"
projects[ding_ting_frontend][download][type] = "git"
projects[ding_ting_frontend][download][url] = "git@github.com:ding2tal/ding_ting_frontend.git"
projects[ding_ting_frontend][download][tag] = "7.x-1.0"

; Libraries

libraries[d3][download][type] = "get"
libraries[d3][download][url] = "https://github.com/mbostock/d3/zipball/master"
libraries[d3][directory_name] = "d3"
libraries[d3][destination] = "libraries"
