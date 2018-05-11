#! /usr/bin/python
import generate_data
import generate_map
import modules.init as init
import sys
##################################################################################
try:
    """ Try to read the configuration file """
    config_file = sys.argv[1]
    init.read_config(config_file)
except Exception as e:
    print('The config file was not found or there are unmatching section! Please make sure of the path and the names of the sections.')
    exit()
else:
    generate_data.main()
    generate_map.main()
    ##############################################################################
