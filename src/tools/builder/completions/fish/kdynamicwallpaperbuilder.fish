# kdynamicwallpaperbuilder(1) completion

complete -f -c kdynamicwallpaperbuilder
complete -c kdynamicwallpaperbuilder -s v -l version -d "Print the version information and quit"
complete -c kdynamicwallpaperbuilder -s h -l help -d "Show help message and quit"
complete -c kdynamicwallpaperbuilder -l help-all -d "Show help message including Qt specific options and quit"

complete -c kdynamicwallpaperbuilder -l quality -d "Specify the quality of the encoded images (from 0 to 100)" -r
complete -c kdynamicwallpaperbuilder -l output -d "Specify the file where the output will be written" -r
complete -c kdynamicwallpaperbuilder -l discard-color-profile -d "Discard embedded color profile"
complete -c kdynamicwallpaperbuilder -l lossless -d "Use lossless coding"
