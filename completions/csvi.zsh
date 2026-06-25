#compdef csvi

_csvgi() {
    local context state line
    _arguments -C \
        '(-s --separator)'{-s,--separator}'[Cell separator]:character:' \
        '(-V --verbose)'{-V,--verbose}'[Verbose logging]' \
        '(-v --version)'{-v,--version}'[Print version]' \
        '(-h --help)'{-h,--help}'[Print help]' \
        '*:CSV file:_files -g "*.csv"'
}

_csvgi "$@"
