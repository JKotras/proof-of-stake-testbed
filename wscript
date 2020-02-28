# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('proof-of-stake-testbed', ['core'])
    module.source = [
        'model/proof-of-stake-testbed.cc',
        'model/blockchain.cpp',
        'helper/proof-of-stake-testbed-helper.cc',
        ]

    module_test = bld.create_ns3_module_test_library('proof-of-stake-testbed')
    module_test.source = [
        'test/proof-of-stake-testbed-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'proof-of-stake-testbed'
    headers.source = [
        'model/proof-of-stake-testbed.h',
        'helper/proof-of-stake-testbed-helper.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

