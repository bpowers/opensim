EXTRA_DIST = infection.gobject.70d624.csv infection.gobject.73369e.csv \
             infection.master.81478f.csv infection.osm \
             rabbit.3cb207.csv rabbit.82b259.csv rabbit_speed_osx.txt\
             run.py speed_test.py test_lex.py test_parse.py \
             test_simulator.py test_variable.py

check-local:
	@PYTHONPATH=$(top_srcdir):$(PYTHONPATH) $(PYTHON) $(srcdir)/run.py
	@rm -rf *.pyc
	@rm -rf $(top_srcdir)/opensim/*.pyc $(top_srcdir)/opensim/engine/*.pyc
