"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class cmd_lcm(object):
    __slots__ = ["linear", "angular"]

    def __init__(self):
        self.linear = 0.0
        self.angular = 0.0

    def encode(self):
        buf = BytesIO()
        buf.write(cmd_lcm._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">dd", self.linear, self.angular))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != cmd_lcm._get_packed_fingerprint():
            raise ValueError("Decode error")
        return cmd_lcm._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = cmd_lcm()
        self.linear, self.angular = struct.unpack(">dd", buf.read(16))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if cmd_lcm in parents: return 0
        tmphash = (0xee60937a86bd6213) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if cmd_lcm._packed_fingerprint is None:
            cmd_lcm._packed_fingerprint = struct.pack(">Q", cmd_lcm._get_hash_recursive([]))
        return cmd_lcm._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)

