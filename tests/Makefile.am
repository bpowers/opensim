check-local:
	@PYTHONPATH=$(top_srcdir):$(PYTHONPATH) $(PYTHON) $(srcdir)/run.py
	@rm -rf *.pyc
	@rm -rf $(top_srcdir)/opensim/*.pyc $(top_srcdir)/opensim/engine/*.pyc
