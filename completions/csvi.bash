# bash completion for csvi
_csvgi_completions()
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="-s -V -v -h --separator --verbose --version --help"

    case "${prev}" in
        -s|--separator)
            return 0
            ;;
        -*)
            COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
            return 0
            ;;
        *)
            COMPREPLY=( $(compgen -f -X '!*.csv' -- "${cur}") $(compgen -f -- "${cur}") )
            return 0
            ;;
    esac
}

complete -F _csvi_completions csvi
