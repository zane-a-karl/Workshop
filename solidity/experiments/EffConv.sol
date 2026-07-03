// SPDX-License-Identifier: UNLICENSED

pragma solidity ^0.5;

contract EffConv {

    function addr_to_bytes(
	address a
    )
	public
        pure
	returns (bytes memory b)
    {
	assembly {
	    let m := mload(0x40)
	    a := and(a, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
	    mstore(add(m, 20), xor(0x140000000000000000000000000000000000000000, a))
	    mstore(0x40, add(m, 52))
	    b := m
	}
    }

    function slow_addr_to_bytes(
	address a
    )
	public
	pure
	returns (bytes memory b)
    {
	return abi.encodePacked(a);
    }
    
} // contract EffConv
