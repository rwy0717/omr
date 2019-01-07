{
    idx = match($0, /^\t+/)
    if(idx > 0){
        indent_width = RLENGTH * 4
        line_content = substr($0, RLENGTH + 1)
        printf("%" indent_width "s%s\n", "", line_content)
    } else {
        print $0
    }
}
