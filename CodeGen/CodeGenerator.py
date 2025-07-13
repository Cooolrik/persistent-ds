# pds - Persistent data structure framework, Copyright (c) 2022 Ulrik Lindahl
# Licensed under the MIT license https://github.com/Cooolrik/pds/blob/main/LICENSE

import sys
import CodeGeneratorHelpers as hlp

# import the ctle code base
ctle_path = '' if len(sys.argv) < 2 else sys.argv[1]
hlp.import_ctle_code_gen(ctle_path)

hlp.run_module('Generators.EntityWriter')
hlp.run_module('Generators.EntityReader')
hlp.run_module('Generators.ElementTypes')
hlp.run_module('Generators.ValueTypes')
hlp.run_module('Generators.DynamicTypes')

