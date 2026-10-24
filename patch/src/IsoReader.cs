// Read-only ISO 9660 reader for the original English Pharaoh's Ascent CD.
// No mounting, external extractor, installer execution, or system changes.
using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

namespace PharaohPatch {
    public sealed class CdReader : IDisposable {
        private readonly FileStream stream;
        private readonly Dictionary<string, Entry> entries = new Dictionary<string, Entry>(StringComparer.OrdinalIgnoreCase);
        private readonly HashSet<uint> visited = new HashSet<uint>();
        private sealed class Entry { public long Offset; public long Size; }
        public CdReader(string path) {
            stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read);
            try {
                byte[] pvd = Read(16 * 2048, 2048);
                if (pvd[0] != 1 || Encoding.ASCII.GetString(pvd, 1, 5) != "CD001" || pvd[6] != 1 || BitConverter.ToUInt16(pvd, 128) != 2048)
                    throw new InvalidDataException("Unsupported ISO: expected an ISO 9660 CD with 2048-byte sectors.");
                Walk(BitConverter.ToUInt32(pvd, 158), BitConverter.ToUInt32(pvd, 166), "", 0);
            } catch { stream.Dispose(); throw; }
        }
        private byte[] Read(long offset, int size) {
            if (offset < 0 || size < 0 || offset > stream.Length - size) throw new InvalidDataException("ISO extent outside file.");
            byte[] data = new byte[size]; stream.Position = offset;
            int total = 0;
            while (total < size) { int n = stream.Read(data, total, size-total); if (n == 0) throw new EndOfStreamException(); total += n; }
            return data;
        }
        private void Walk(uint sector, uint size, string prefix, int depth) {
            if (depth > 12 || size > 8 * 1024 * 1024 || !visited.Add(sector)) throw new InvalidDataException("Invalid ISO directory tree.");
            byte[] data = Read((long)sector * 2048, (int)size);
            for (int pos = 0; pos < data.Length;) {
                int length = data[pos];
                if (length == 0) { pos = (pos / 2048 + 1) * 2048; continue; }
                if (length < 34 || pos + length > data.Length || data[pos+32] + 33 > length) throw new InvalidDataException("Invalid ISO directory record.");
                int nameLength = data[pos+32];
                if (!(nameLength == 1 && data[pos+33] < 2)) {
                    string name = Encoding.ASCII.GetString(data, pos+33, nameLength).Split(';')[0];
                    if (String.IsNullOrEmpty(name) || name == "." || name == ".." || name.IndexOfAny(Path.GetInvalidFileNameChars()) >= 0 || name.EndsWith(" "))
                        throw new InvalidDataException("Unsafe ISO filename.");
                    string path = prefix + name;
                    uint extent = BitConverter.ToUInt32(data, pos+2), bytes = BitConverter.ToUInt32(data, pos+10);
                    if ((data[pos+25] & 2) != 0) Walk(extent, bytes, path + "/", depth+1);
                    else {
                        if ((data[pos+25] & 128) != 0) throw new InvalidDataException("Multi-extent ISO files are unsupported.");
                        long offset = (long)extent * 2048;
                        if (offset > stream.Length - bytes || entries.Count >= 10000 || entries.ContainsKey(path)) throw new InvalidDataException("Invalid or duplicate ISO file.");
                        entries.Add(path, new Entry { Offset = offset, Size = bytes });
                    }
                }
                pos += length;
            }
        }
        public void Extract(string name, string destination, long expectedSize) {
            Entry entry;
            if (!entries.TryGetValue(name, out entry)) throw new FileNotFoundException("Required CD file missing: " + name);
            if (entry.Size != expectedSize) throw new InvalidDataException("Unexpected CD file size: " + name);
            using (FileStream output = new FileStream(destination, FileMode.CreateNew, FileAccess.Write)) {
                stream.Position = entry.Offset; byte[] buffer = new byte[65536]; long remaining = entry.Size;
                while (remaining > 0) {
                    int n = stream.Read(buffer, 0, (int)Math.Min(buffer.Length, remaining));
                    if (n == 0) throw new EndOfStreamException(); output.Write(buffer, 0, n); remaining -= n;
                }
            }
        }
        public void Dispose() { stream.Dispose(); }
    }
}
