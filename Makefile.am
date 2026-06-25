SUBDIRS = src/lib src/bin tests

dist_man_MANS = man/csvi.1

EXTRA_DIST = \
             AGENTS.md \
             bootstrap \
             build-installer \
             docs/architecture.md \
             docs/build.md \
             docs/tutorial.md \
             completions/csvi.bash \
             completions/csvi.zsh \
             installer-template \
             packaging/nfpm.yaml \
             packaging/build-packages.sh

AM_DISTCHECK_CONFIGURE_FLAGS = --enable-debug

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(datadir)/bash-completion/completions
	$(INSTALL_DATA) $(srcdir)/completions/csvi.bash \
	  $(DESTDIR)$(datadir)/bash-completion/completions/csvi
	$(MKDIR_P) $(DESTDIR)$(datadir)/zsh/site-functions
	$(INSTALL_DATA) $(srcdir)/completions/csvi.zsh \
	  $(DESTDIR)$(datadir)/zsh/site-functions/_csvi

uninstall-local:
	rm -f $(DESTDIR)$(datadir)/bash-completion/completions/csvi
	rm -f $(DESTDIR)$(datadir)/zsh/site-functions/_csvi
