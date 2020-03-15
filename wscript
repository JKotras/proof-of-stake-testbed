# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    module = bld.create_ns3_module('proof-of-stake-testbed', ['internet', 'config-store','stats', 'network', 'core'])
    module.source = [
        'model/proof-of-stake-testbed.cc',
        'model/blockchain.cc',
        'model/blockchain-node.cc',
        'model/ouroboros-node.cc',
        'helper/proof-of-stake-testbed-helper.cc',
        'utils/rsa.cc',
        ]

    module_test = bld.create_ns3_module_test_library('proof-of-stake-testbed')
    module_test.source = [
        'test/proof-of-stake-testbed-test-suite.cc',
        'test/blockchain-test.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'proof-of-stake-testbed'
    headers.source = [
        'model/proof-of-stake-testbed.h',
        'model/blockchain.h',
        'model/blockchain-node.h',
        'model/ouroboros-node.h',
        'helper/proof-of-stake-testbed-helper.h',
        'utils/rsa.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    bld.ns3_python_bindings()

