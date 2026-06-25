# bash completion for csvi
_csvgi_completions()
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="-s -C -G -H -V -v -h --separator --color --grid --header --verbose --version --help"

    case "${prev}" in
        -s|--separator)
            return 0
            ;;
        --color)
            COMPREPLY=( $(compgen -W "auto never always" -- "${cur}") )
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
