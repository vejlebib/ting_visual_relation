api = 2
core = 7.x

; Ding2 modules

projects[ding_ting_frontend][type] = "module"
projects[ding_ting_frontend][download][type] = "git"
projects[ding_ting_frontend][download][url] = "git@github.com:ding2tal/ding_ting_frontend.git"
projects[ding_ting_frontend][download][tag] = "7.x-1.0"

; Libraries

libraries[d3][download][type] = "get"
libraries[d3][download][url] = "https://github.com/mbostock/d3/zipball/master"
libraries[d3][directory_name] = "d3"
libraries[d3][destination] = "libraries"
