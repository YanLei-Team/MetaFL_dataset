/*
probe_match.h - Header file for probe matching
written by JIANG Hui,
Institute for Computational and Mathematical Engineering, Stanford University
May, 2007 -
*/

#ifndef PROBE_MATCH_H
///Define this macro to prevent from including this header file more than once.
#define PROBE_MATCH_H

/*
probe.h - Header file for probe manipulation
written by JIANG Hui,
Institute for Computational and Mathematical Engineering, Stanford University
May, 2007 -
*/

#ifndef PROBE_H
///Define this macro to prevent from including this header file more than once.
#define PROBE_H

#include "type.h"
#include "string_operation.h"

const char DNA_C[4] = {'A', 'C', 'G', 'T'};
const int MAX_PROBE_LEN = 32;
int probe_len = 25;

inline string tostring(const int64 key, int len = probe_len){
    int i;
    int64 temp = key;
    char buf[MAX_PROBE_LEN + 1];
    for (i = 0; i < len; i++) {
        int last_char = (int)(temp & 3);
        buf[len - i - 1] = DNA_C[last_char];
        temp >>= 2;
    }
    buf[len] = 0;
    return string(buf);
}

inline int64 tonumber(const string &probe){
    int i;
    int64 result = 0;
    int probe_len = (int)probe.length();
    for (i = 0; i < probe_len; i++) {
        result <<= 2;
        char ch = probe[i];
        int j;
        for (j = 0; j < 4; j++) {
            if (ch == DNA_C[j]) break;
        }
        if (j == 4) {
            return -1;
        }
        result |= j;
    }
    return result;
}

inline char random_NT() {
    return (DNA_C[rand() % 4]);
}

inline bool fix_N_probe(string &probe, int max_N = 0) {
    int i;
    int probe_len = (int)probe.length();
    int num_fixed = 0;
    bool has_N = false;
    for (i = 0; i < probe_len; i++) {
        char ch = probe[i];
        int j;
        for (j = 0; j < 4; j++) {
            if (ch == DNA_C[j]) break;
        }
        if (j == 4) {
            has_N = true;
            if (num_fixed < max_N) {
                num_fixed++;
                if (ch == 'N' || ch == 'n' || ch == '.') probe[i] = random_NT();
            }
        }
    }
    return has_N;
}

inline void fix_short_probe(string &probe, int max_N = 0) {
    int len = (int)probe.length();
    if (len >= probe_len) return;
    while ((int)probe.length() < probe_len) probe = probe + "N";
    if (probe_len - len <= max_N) fix_N_probe(probe, max_N);
}

inline void local_alignment(string &probe1, string &probe2, int num_insdel) {
    probe1 = toupper(probe1);
    probe2 = toupper(probe2);

    int i, j;
    int len1 = (int)probe1.length(), len2 = (int)probe2.length();
    string seq1, seq2;

    if (num_insdel == 0) {
        __ASSERT(len1 == len2, "internal error, len1 != len2.\n");
        for (i = 0; i < len1; i++) {
            if (probe1[i] == probe2[i]) {
                seq1.push_back(probe1[i]);
                seq2.push_back(probe2[i]);
            } else {
                seq1.push_back((char)tolower(probe1[i]));
                seq2.push_back((char)tolower(probe2[i]));
            }
        }
    } else {
        vector<vector<int> > d;
        int dim = 2 * max(len1, len2);
        d.resize(dim);
        for (i = 0; i < (int)d.size(); i++) d[i].resize(dim);
        for (i = 0; i <= len1; i++) d[i][0] = i;
        for (j = 0; j <= len2; j++) d[0][j] = j;

        for (i = 1; i <= len1; i++) {
            for (j = 1; j <= len2; j++) {
                int cost = (probe1[i-1] == probe2[j-1]) ? 0 : 1;
                d[i][j] = min(d[i-1][j]+1, d[i][j-1]+1);
                d[i][j] = min(d[i][j], d[i-1][j-1]+cost);
            }
        }
        i = len1;
        j = len2;
        while (i > 0 || j > 0) {
            if (i > 0 && j > 0 && d[i][j] == d[i-1][j-1] + 1) {
                i--;
                j--;
                seq1.insert(seq1.begin(), (char)tolower(probe1[i]));
                seq2.insert(seq2.begin(), (char)tolower(probe2[j]));
            } else if (i > 0 && d[i][j] == d[i-1][j] + 1) {
                i--;
                seq1.insert(seq1.begin(), (char)tolower(probe1[i]));
                seq2.insert(seq2.begin(), '_');
            } else if (j > 0 && d[i][j] == d[i][j-1] + 1) {
                j--;
                seq1.insert(seq1.begin(), '_');
                seq2.insert(seq2.begin(), (char)tolower(probe2[j]));
            } else if (i > 0 && j > 0 && d[i][j] == d[i-1][j-1]) {
                i--;
                j--;
                seq1.insert(seq1.begin(), probe1[i]);
                seq2.insert(seq2.begin(), probe2[j]);
            } else {
                panic("internal error: _edit_dist_pos\n");
            }
        }
    }
    probe1 = seq1;
    probe2 = seq2;
}

inline int _edit_dist3(const string &probe1, const string &probe2, int num_mismatch, int num_insdel, int &len) {
    int i, j;
    vector<vector<int> > d;
    int dim = max((int)probe1.length(), (int)probe2.length()) + num_insdel + 1;
    d.resize(dim);
    for (i = 0; i < (int)d.size(); i++) d[i].resize(dim);

    for (i = 0; i <= num_insdel; i++) d[i][0] = d[0][i] = i;
    int probe1_len = (int)probe1.length(), probe2_len = (int)probe2.length();
    for (i = num_insdel + 1; i <= probe1_len; i++) d[i][i - num_insdel - 1] = num_mismatch + 1;
    for (i = num_insdel + 1; i <= probe2_len; i++) d[i - num_insdel - 1][i] = num_mismatch + 1;

    int min_d = probe2_len;
    int min_d_i = 0;
    for (i = 1; i <= probe1_len; i++) {
        j = (i <= num_insdel) ? 1 : i - num_insdel;
        while (j <= i + num_insdel && j <= probe2_len) {
            int cost = (probe1[i-1] == probe2[j-1]) ? 0 : 1;
            d[i][j] = min(d[i-1][j]+1, d[i][j-1]+1);
            d[i][j] = min(d[i][j], d[i-1][j-1]+cost);
            if (j == probe2_len && d[i][j] < min_d) {
                min_d = d[i][j];
                min_d_i = i;
            }
            if (i == j && d[i][j] > num_mismatch + num_insdel) return (num_mismatch + 1);
            j++;
        }
    }
    if (min_d == d[probe2_len][probe2_len]) len = probe2_len;
    else len = min_d_i;
    return min_d;
}

inline int _edit_dist2(const string &probe1, const string &probe2, int num_mismatch, int num_insdel, int &len) {
    int d[MAX_PROBE_LEN*2][MAX_PROBE_LEN+1];
    int i, j;
    for (i = 0; i <= num_insdel; i++) d[i][0] = d[0][i] = i;
    for (i = num_insdel + 1; i <= len; i++) d[i][i - num_insdel - 1] = num_mismatch + 1;
    for (i = num_insdel + 1; i <= probe_len; i++) d[i - num_insdel - 1][i] = num_mismatch + 1;

    int min_d = probe_len;
    int min_d_i = 0;
    for (i = 1; i <= len; i++) {
        j = (i <= num_insdel) ? 1 : i - num_insdel;
        while (j <= i + num_insdel && j <= probe_len) {
            int cost = (probe1[i-1] == probe2[j-1]) ? 0 : 1;
            d[i][j] = min(d[i-1][j]+1, d[i][j-1]+1);
            d[i][j] = min(d[i][j], d[i-1][j-1]+cost);
            if (j == probe_len && d[i][j] < min_d) {
                min_d = d[i][j];
                min_d_i = i;
            }
            if (i == j && d[i][j] > num_mismatch + num_insdel) return (num_mismatch + 1);
            j++;
        }
    }
    if (min_d == d[probe_len][probe_len]) len = probe_len;
    else len = min_d_i;
    return min_d;
}

inline int _edit_dist(const string &probe1, const string &probe2) {
    int d[MAX_PROBE_LEN+1][MAX_PROBE_LEN+1];
    int i, j;
    for (i = 0; i <= probe_len; i++) d[i][0] = d[0][i] = i;

    for (i = 1; i <= probe_len; i++) {
        for (j = 1; j <= probe_len; j++) {
            int cost = (probe1[i-1] == probe2[j-1]) ? 0 : 1;
            d[i][j] = min(d[i-1][j]+1, d[i][j-1]+1);
            d[i][j] = min(d[i][j], d[i-1][j-1]+cost);
        }
    }
    return d[probe_len][probe_len];
}

inline vector<uchar> _edit_dist_pos(const string &probe1, const string &probe2) {
    int d[MAX_PROBE_LEN*2][MAX_PROBE_LEN*2];
    int i, j;
    int len1 = (int)probe1.length(), len2 = (int)probe2.length();
    for (i = 0; i <= len1; i++) d[i][0] = i;
    for (j = 0; j <= len2; j++) d[0][j] = j;

    for (i = 1; i <= len1; i++) {
        for (j = 1; j <= len2; j++) {
            int cost = (probe1[i-1] == probe2[j-1]) ? 0 : 1;
            d[i][j] = min(d[i-1][j]+1, d[i][j-1]+1);
            d[i][j] = min(d[i][j], d[i-1][j-1]+cost);
        }
    }
    vector<uchar> result;
    result.clear();
    i = len1;
    j = len2;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && d[i][j] == d[i-1][j-1] + 1) {
            i--;
            j--;
            result.insert(result.begin(), (uchar)(j));
        } else if (i > 0 && d[i][j] == d[i-1][j] + 1) {
            i--;
            result.insert(result.begin(), (uchar)(j));
        } else if (j > 0 && d[i][j] == d[i][j-1] + 1) {
            j--;
            result.insert(result.begin(), (uchar)(j));
        } else if (i > 0 && j > 0 && d[i][j] == d[i-1][j-1]) {
            i--;
            j--;
        } else {
            panic("internal error: _edit_dist_pos\n");
        }
    }
    return result;
}

inline int _edit_dist2(const int64 key1, const int64 key2, int num_mismatch, int num_insdel, int &len) {
    return _edit_dist2(tostring(key1, len), tostring(key2), num_mismatch, num_insdel, len);
}

inline int _edit_dist(const int64 key1, const int64 key2) {
    return _edit_dist(tostring(key1), tostring(key2));
}

inline vector<uchar> _edit_dist_pos(const int64 key1, const int64 key2, int len=probe_len) {
    return _edit_dist_pos(tostring(key1, len), tostring(key2));
}

inline int64 get_reverse(const int64 key, int len = probe_len){
    int64 result = 0;
    int64 temp = (~key)&((((int64)(1))<<(len*2))-1);
    int i;
    for (i = 0; i < len; i++) {
        result <<= 2;
        result |= (temp & 3);
        temp >>= 2;
    }
    return result;
}

inline string get_reverse(const string &probe){
    int probe_len = (int)probe.length();
    return (tostring(get_reverse(tonumber(probe), probe_len), probe_len));
}

inline string get_reverse2(const string &probe){
    string result = "";
    string temp = toupper(probe);
    for (int i = (int)probe.length() - 1; i >= 0; i--) {
        if (probe[i] == 'A') result += "T";
        else if (probe[i] == 'T') result += "A";
        else if (probe[i] == 'C') result += "G";
        else if (probe[i] == 'G') result += "C";
        else result += probe[i];
    }
    return result;
}

static int mismatch_table[65536];

inline void build_mismatch_table() {
    int i;
    for (i = 0; i < 65536; i++) {
        int result = 0;
        int temp = i;
        while (temp) {
            if (temp & 3) result++;
            temp >>= 2;
        }
        mismatch_table[i] = result;
    }
}

inline int _mismatch2(const string &seq1, const string &seq2, int num_mismatch) {
    int result = 0;
    for (int i = 0; i < min((int)seq1.length(), (int)seq2.length()); i++) {
        if (seq1[i] != seq2[i]) {
            result++;
            if (result > num_mismatch) return result;
        }
    }
    if ((int)seq1.length() > (int)seq2.length()) result += (int)seq1.length() - (int)seq2.length();
    else result += (int)seq2.length() - (int)seq1.length();
    return result;
}

inline int _mismatch2(const int64 key1, const int64 key2, int num_mismatch) {
    int64 temp = key1 ^ key2;
    int result = mismatch_table[temp & 65535];
    if (result > num_mismatch) return result;
    result += mismatch_table[(temp >> 16) & 65535];
    if (result > num_mismatch) return result;
    result += mismatch_table[(temp >> 32) & 65535];
    if (result > num_mismatch) return result;
    result += mismatch_table[(temp >> 48) & 65535];
    return result;
}

inline int _mismatch(const int64 key1, const int64 key2) {
    int64 temp = key1 ^ key2;
    int result = 0;
    int i;
    for (i = 0; i < probe_len; i++) {
        if ((temp & 3) != 0) result++;
        temp >>= 2;
    }
    return result;
}

inline vector<uchar> _mismatch_pos(const int64 key1, const int64 key2) {
    int64 temp = key1 ^ key2;
    int i;
    vector<uchar> result;
    result.clear();
    for (i = 0; i < probe_len; i++) {
        if ((temp & 3) != 0) result.insert(result.begin(), (uchar)(probe_len - 1 - (i)));
        temp >>= 2;
    }
    return result;
}

inline void correct_key(int64 &key, int len = probe_len) {
    key &= (((int64)(1) << (len * 2)) - 1);
}

inline int mismatch(const int64 key1, const int64 key2, int num_mismatch, int num_insdel, int &len){
    if (num_insdel > 0) return _edit_dist2(key1, key2, num_mismatch, num_insdel, len);
    else return _mismatch2(key1, key2, num_mismatch);
}

inline int mismatch(const string &seq1, const string &seq2, int num_mismatch, int num_insdel, int &len) {
    if (num_insdel > 0) return _edit_dist3(seq1, seq2, num_mismatch, num_insdel, len);
    else return _mismatch2(seq1, seq2, num_mismatch);
}

inline vector<uchar> mismatch_pos(const int64 key1, const int64 key2, int num_insdel, int &len){
    if (num_insdel > 0) return _edit_dist_pos(key1, key2, len);
    else {
        __ASSERT(len == probe_len, "internal error: wrong probe len.\n");
        return _mismatch_pos(key1, key2);
    }
}

#endif //PROBE_H
#include "system_utils.h"
#include "timer.h"
#include "file_operation.h"
#include "fasta.h"

const int max_mismatch = 5;
const int max_insdel = 5;

#ifdef __DEBUG
    static int64 num_total_search_step = 0;
    static int64 num_search = 0;
    static int64 num_max_search_step = 0;

    static int64 total_jump1 = 0, total_jump2 = 0;
    static int64 max_jump1 = 0, max_jump2 = 0;
    static int64 total_insertion = 0, total_detection = 0;
#endif

class search_result_struct{
public:
    int trans_id;
    int trans_coord;
    int64 trans_key;
    int trans_key_len;
    string trans_seq;
    int probe_index;
    int num_mismatch;
    bool reverse_strand;

    bool reported;
};

typedef void search_result_handler(vector<search_result_struct> &results);

class probe_list;

class probe_info{
public:
    string id;
    string seq;
    int64 key;
    bool valid;
    bool rematch;
    int repeat; //-1 unique, >=0 identical to another probe
};

bool operator<(const probe_info& a, const probe_info& b) {
    return a.key < b.key;
}

class probe_match{
public:
    bool verbose;
    bool exact_mismatch;
    int num_mismatch;
    bool search_repeats;
    bool search_both_strands;
    bool duplicate_probes;
    bool cluster_identical_probes;

    int cut_start, cut_end;
    bool match_shorter_probes;
    bool match_N;

    bool filter_results;
    bool filter_selected_probes;
    string filter_selected_probes_filename;
    bool output_filtered_probes;
    bool filter_low_quality_probes;
//for hash
    bool use_hash;
    bool hash_1bp_mismatch;
//for probe lists
    int limit_num_part;
//for probe_list
    bool fast_index;
    double fast_index_fraction;
    bool interpolation_search;
    bool store_key;
    bool shift_mask;
//for insdel
    int num_insdel;
//for output
    bool zero_indexed;

    string probe_input_file_name;

    size_t used_memory;
    size_t available_memory;
    size_t extra_memory_per_probe;
    int num_probes;
    int num_valid_probes;
    int nn;
    uint hash_magic;
    uint* hash_table;
    vector<probe_info> probes;
    vector<string> transcripts;
    vector<probe_list*> probe_lists;
    probe_match();
    ~probe_match();
    bool initialize();
    bool initialize(const string &fa_file_name);
    bool initialize(vector<string> &probe_sequences, vector<string> &probe_tags);
    bool initialize(vector<string> &probe_sequences) {
        vector<string> probe_tags;
        int n = (int)probe_sequences.size();
        probe_tags.resize(n);
        for (int i = 0; i < n; i++) probe_tags[i] = int2str(i + 1);
        return initialize(probe_sequences, probe_tags);
    }
    bool search_key(const string &key, vector<uint> &indexes) { return search_key(tonumber(key), indexes);}
    bool search_key(const int64 key, vector<uint> &indexes);
    void search_results_prehandler(vector<search_result_struct> &results, search_result_handler handler);
    bool search_file(const string &fa_file_name, search_result_handler handler);
//for hash
    inline int64 get_probe(const uint probe_index);
    inline bool hash_insert_probe(const uint probe_index, const uint pos, const uint changeto);
    inline uint hash_detect_probe(const int64 key, vector<uint> &indexes);
};

class probe_list{
public:
    probe_match* pm;
    vector<int64> masks;
    int64 mask0_table[65536][4];
    int mask0_shift0, mask0_shift1, mask0_shift2;
    int fast_index_shift;
    int size_fast_index;
    int* fast_indexes;
    vector<int64> key_list;
    int* index_list;

    probe_list(){
        fast_index_shift = 0;
        masks.clear();
        index_list = NULL;
        fast_indexes = NULL;
    }

    ~probe_list(){
        if (NULL != index_list) delete[] index_list;
        if (NULL != fast_indexes) delete[] fast_indexes;
    }

    inline int64 shift_key(const int64 key, const int64 mask) {
        int64 result = 0;
        int64 m = mask;
        int64 k = key;
        int64 pos = 1;
        while (m) {
            result |= (k & m & 1) * pos;
            pos <<= (m & 1);
            m >>= 1;
            k >>= 1;
        }
        return result;
    }

    inline int64 shift_mask0_key(const int64 key) {
        int64 result = 0;
        result += mask0_table[key & 65535][0];
        result += (mask0_table[(key >> 16) & 65535][1] << mask0_shift0);
        result += (mask0_table[(key >> 32) & 65535][2] << mask0_shift1);
        result += (mask0_table[(key >> 48) & 65535][3] << mask0_shift2);
        return result;
    }

    inline void build_mask0_tables(){
        int i, j;
        for (i = 0; i < 65536; i++) {
            for (j = 0; j < 4; j++) {
                int64 submask = ((masks[0] >> (j * 16)) & 65535);
                mask0_table[i][j] = shift_key(i, submask);
            }
        }
        mask0_shift0 = count1(masks[0] & 65535);
        mask0_shift1 = mask0_shift0 + count1((masks[0] >> 16) & 65535);
        mask0_shift2 = mask0_shift1 + count1((masks[0] >> 32) & 65535);
    }

    inline int count1(int64 key) {
        int64 k = key;
        int result = 0;

        while (k) {
            result += int(k&1);
            k >>= 1;
        }
        return result;
    }

    inline int64 mask0_key(const int64 key) {
        if (pm->shift_mask) return shift_mask0_key(key);
        else return masks[0] & key;
    }

    inline int64 mask_key(const int64 key, const int64 mask) {
        if (pm->shift_mask) return (shift_key(key, mask));
        else return mask & key;
    }

    inline void set_mask(vector<int64> &_masks){
        masks = _masks;
        build_mask0_tables();
        if ((int64)(masks.size()) * pm->num_probes > ((int64)(1) << 31)) panic("internal error: out of integer bound.");
        int i, j;
        vector<pair<int64, int> > temp_probe_list;
        for (i = 0; i < pm->num_probes; i++)
            for (j = 0; j < (int)masks.size(); j++) {
                if (pm->probes[i].valid) temp_probe_list.push_back(pair<int64, int>(mask_key(pm->probes[i].key, masks[j]), j * pm->num_probes + i));
            }
        __ASSERT((int)temp_probe_list.size() == num_items(), "internal error: wrong num_items.\n");
        sort(temp_probe_list.begin(), temp_probe_list.end());
        index_list = new int[num_items()];
        for (i = 0; i < num_items(); i++) {
            index_list[i] = temp_probe_list[i].second;
            if (pm->store_key) key_list.push_back(temp_probe_list[i].first);
        }
        temp_probe_list.clear();
        if (pm->fast_index) {
            int64 temp = shift_key(masks[0], masks[0]);
            temp++;
            fast_index_shift = 0;
            while(temp > num_items() / pm->fast_index_fraction){
                temp >>= 1;
                fast_index_shift++;
            }
            size_fast_index = (int)(shift_key(masks[0], masks[0]) >> fast_index_shift) + 1;
            fast_indexes = new int[size_fast_index];
            fast_indexes[0] = 0;
            int current = 1;
            for (i = 0; i < num_items(); i++) {
                int temp;
                if (masks.size() > 1) {
                    __ASSERT(pm->num_insdel > 0 && pm->shift_mask, "internal error: masks.size > 0.\n");
                }
                if (pm->shift_mask) temp = (int)(get_key(i) >> fast_index_shift);
                else temp = (int)(shift_key(pm->probes[get_probe_index(i)].key, masks[0]) >> fast_index_shift);
                if (temp >= current) {
                    for (j = current; j <= temp; j++) {
                        fast_indexes[j] = i;
                    }
                    current = temp+1;
                }
            }
            for (j = current; j < size_fast_index; j++) fast_indexes[j] = num_items() - 1;
        }
    }

    inline int64 get_key(const int index) {
        if (pm->store_key) return key_list[index];
        else return (masks.size() > 1) ? mask_key(get_probe(index), get_mask(index)) : mask_key(pm->probes[index_list[index]].key, masks[0]);
    }

    inline int64 get_probe(const int index) {
        return pm->probes[get_probe_index(index)].key;
    }

    inline int64 get_mask(const int index) {
        return masks[get_mask_index(index)];
    }

    inline int get_probe_index(const int index) {
        return (masks.size() > 1) ? index_list[index] % pm->num_probes : index_list[index];
    }

    inline int get_mask_index(const int index) {
        if (masks.size() > 1) {
            return index_list[index] / pm->num_probes;
        } else return 0;
    }

    inline int num_items() {
        return pm->num_valid_probes * (int)masks.size();
    }

    inline bool search_probe(const int64 probe, vector<uint> &indexes){
        int left, right, mid;
        int64 subprobe = (probe >> (pm->num_insdel * 2));
        int64 key = mask0_key(subprobe);

        if (pm->fast_index) {
            int temp = int(shift_mask0_key(subprobe) >> fast_index_shift);
            left = fast_indexes[temp];
            if (temp < size_fast_index - 1) right = fast_indexes[temp + 1];
            else right = num_items() - 1;
        } else {
            left = 0;
            right = num_items() - 1;
        }

        mid = left;
#ifdef __DEBUG
        int64 count_search_step = 0;
#endif
        while((get_key(left)) < key && (get_key(right)) >= key) {
            __ASSERT((left < right), "internal error in search");
#ifdef __DEBUG
            count_search_step++;
#endif
            if (pm->interpolation_search) {
                mid = left + (int)((((double)key - get_key(left)) * (right - left)) / (get_key(right) - get_key(left)));
                __ASSERT((left <= mid && mid <= right), "internal error in search");
            } else mid = (left + right) / 2;
            if ((get_key(mid)) > key)
                right = mid - 1;
            else if ((get_key(mid)) < key)
                left = mid + 1;
            else {
                left = mid;
                break;
            }
        }
#ifdef __DEBUG
        num_total_search_step += count_search_step;
        num_search++;
        if (count_search_step > num_max_search_step) num_max_search_step = count_search_step;
#endif
        if ((get_key(left)) != key)
            return false;
        else {
            while (left > 0 && (get_key(left-1)) == key)
                left--;
            while (left < num_items() && (get_key(left)) == key) {
                int len = probe_len + pm->num_insdel;
                int score = mismatch(probe, get_probe(left), pm->num_mismatch, pm->num_insdel, len);

                if (pm->exact_mismatch && score == pm->num_mismatch) indexes.push_back(get_probe_index(left));
                else if (score <= pm->num_mismatch) indexes.push_back(get_probe_index(left));

                left++;
            }
            return true;
        }
    }
};

inline uint joaat_hash(const uchar *key, const uint len)
 {
     uint hash = 0;
     uint i;

     for (i = 0; i < len; i++) {
         hash += key[i];
         hash += (hash << 10);
         hash ^= (hash >> 6);
     }
     hash += (hash << 3);
     hash ^= (hash >> 11);
     hash += (hash << 15);
     return hash;
 }

inline int64 change_probe(const int64 probe, const uint pos, const uint changeto){
    return((probe & (((int64)(1) << (probe_len * 2)) - 1 - ((int64)(3) << (pos * 2)))) | ((int64)(changeto) << (pos * 2)));
}

inline uint encode_probe_index(const uint probe_index, const uint pos, const uint changeto){
    return (probe_index | (pos << 25) | (changeto << 30));
}

inline uint decode_probe_index(const uint probe_index){
    return (probe_index & ((1 << 25) - 1));
}

inline int64 probe_match::get_probe(const uint probe_index){
    uint index = decode_probe_index(probe_index);
    uint pos = (probe_index >> 25) & 31;
    uint changeto = (probe_index >> 30) & 3;
    __ASSERT(index < (uint)num_probes,"get probe error");
    __ASSERT(pos < (uint)probe_len, "get probe error");
    return (change_probe(probes[index].key, pos, changeto));
}

inline bool probe_match::hash_insert_probe(const uint probe_index, const uint pos, const uint changeto) {
    uint encoded_probe_index = encode_probe_index(probe_index, pos, changeto);
    int64 temp1 = get_probe(encoded_probe_index);
    uint index = joaat_hash((uchar*)(&temp1), 8) % nn;
    uint index_inc = 1;
    uint origin = index;
#ifdef __DEBUG
    int64 jump = 1;
#endif
    while (hash_table[index] != (uint)(-1)) {
#ifdef __DEBUG
        jump++;
#endif
        index += index_inc;
        index %= nn;
        if (origin == index)
            return false;
    }
#ifdef __DEBUG
    if (jump > max_jump1) max_jump1 = jump;
    total_jump1 += jump;
    total_insertion++;
#endif
    hash_table[index] = encoded_probe_index;
    return true;
}

inline uint probe_match::hash_detect_probe(const int64 key, vector<uint> &indexes) {
    uint index = joaat_hash((uchar*)(&key), 8) % nn;
    uint index_inc = 1;
    uint origin = index;
    indexes.clear();
#ifdef __DEBUG
    int64 jump = 1;
#endif
    while (hash_table[index] != (uint)(-1)) {
#ifdef __DEBUG
        jump++;
#endif
        int64 temp;
        temp = get_probe(hash_table[index]);

        if (temp == key) {
            indexes.push_back(hash_table[index]);
        }
        index += index_inc;
        index %= nn;
        if (origin == index) panic("circle in hash table");
    }
#ifdef __DEBUG
    if (jump > max_jump2) max_jump2 = jump;
    total_jump2 += jump;
    total_detection++;
#endif
    if (indexes.size() == 0)
        return (uint)(-1);
    else
        return indexes[0];
}

inline probe_match::probe_match(){
    verbose = true;
    limit_num_part = 0;
    exact_mismatch = false;
    search_repeats = true;
    search_both_strands = true;
    duplicate_probes = true;
    cluster_identical_probes = true;

    cut_start = cut_end = 0;
    match_shorter_probes = false;
    match_N = true;

    filter_results = true;
    filter_selected_probes = false;
    filter_selected_probes_filename = "";
    output_filtered_probes = false;
    filter_low_quality_probes = false;

    use_hash = false;
    hash_1bp_mismatch = true;
    hash_table = NULL;
    num_mismatch = 0;
    num_probes = 0;
    num_valid_probes = 0;

    fast_index = true;
    fast_index_fraction = 4;
    interpolation_search = false;
    shift_mask = false;
    store_key = true;

    used_memory = 0;
    available_memory = 0;
    extra_memory_per_probe = 0;
    num_insdel = 0;

    zero_indexed = false;
}

inline probe_match::~probe_match(){
    int i;
    for (i = 0; i < (int)probe_lists.size(); i++)
        delete probe_lists[i];
    if (use_hash) {
        if (hash_table != NULL) {
            delete[] hash_table;
            hash_table = NULL;
        }
    }
}

inline static void _insdel_masks(vector<int64> &masks, int64 mask, int num_insdel){
    if (num_insdel == 0) return;
    int i;
    for (i = 0; i < probe_len; i++) {
        if ((mask & ((int64)(3) << (i * 2))) == 0) {
            int64 left = ((mask >> (i * 2)) << (i * 2));
            int64 right = mask - left;
            int64 new_mask = left + (right >> 2);
            if (find(masks.begin(), masks.end(), new_mask) == masks.end()) masks.push_back(new_mask);
            _insdel_masks(masks, new_mask, num_insdel-1);
            new_mask = left + (right << 2);
            if (find(masks.begin(), masks.end(), new_mask) == masks.end()) masks.push_back(new_mask);
            _insdel_masks(masks, new_mask, num_insdel-1);
        }
    }
}

inline bool probe_match::initialize(){
    int i, j, k;

    for (i = 0; i < (int)probe_lists.size(); i++)
        delete probe_lists[i];
    if (use_hash) {
        if (hash_table != NULL) {
            delete[] hash_table;
            hash_table = NULL;
        }
    }

    num_probes = (int)probes.size();
    if(num_probes <= 0) panic("no probe.\n");
    num_valid_probes = 0;
    for (i = 0; i < num_probes; i++) {
        if (probes[i].valid) num_valid_probes++;
    }
    if(num_valid_probes <= 0) panic("no valid probe.\n");
    if (verbose) cout << num_probes << " probes imported. " << num_valid_probes << " are valid." << endl;
    if (verbose) cout << "checking resources...";

    if (is32system()) {
        if (verbose) cout << "32-bit version." << endl;
    } else {
        if (verbose) cout << "64-bit version." << endl;
    }
    if (available_memory == 0) available_memory = get_available_memory_size();
    if (verbose) cout << "available memory: " << (int)(available_memory >> 20) << "MB.\n";

    build_mismatch_table();

    if (!search_both_strands) duplicate_probes = false;
    if (num_insdel > 0) {
        use_hash = false;
        shift_mask = true;
        if (search_both_strands) duplicate_probes = true;
    } else {
        filter_results = false;
    }

    if (num_mismatch > max_mismatch) panic("num_mismatch > max_mismatch.\n");
    if (num_insdel > max_insdel) panic("num_insdel > max_insdel.\n");
    if (num_insdel > num_mismatch) panic("num_insdel > num_mismatch.\n");

    if (duplicate_probes) {
        if (verbose) cout << "generating reversed probes for searching reverse strand\n";
        for (i = 0; i < num_probes; i++) {
            probe_info temp_probe_info = probes[i];
            if (temp_probe_info.valid) {
                if (temp_probe_info.rematch && (int)temp_probe_info.seq.length() > probe_len) {
                    string probe = get_reverse2(temp_probe_info.seq);
                    probe = probe.substr(0, probe_len);
                    if (match_N) {
                        fix_N_probe(probe, num_mismatch);
                    }
                    temp_probe_info.key = tonumber(probe);
                    if (temp_probe_info.key == -1) {
                        temp_probe_info.valid = false;
                        probes[i].valid = false;
                        num_valid_probes--;
                    }
                } else {
                    temp_probe_info.key = get_reverse(temp_probe_info.key);
                }
            }
            probes.push_back(temp_probe_info);
        }
        __ASSERT(num_probes * 2 == (int)probes.size(), "internal error: wrong num_probes.\n");
        num_probes = (int)probes.size();
        int old_num_valid_probes = num_valid_probes;
        num_valid_probes = 0;
        for (i = 0; i < num_probes; i++) {
            if (probes[i].valid) num_valid_probes++;
        }
        __ASSERT(old_num_valid_probes * 2 == num_valid_probes, "internal error: wrong num_valid_probes.\n");
        if (verbose) cout << "now " << num_probes << " probes. " << num_valid_probes << " are valid." << endl;
    }

    used_memory = probes.size() * (sizeof(probe_info) + probes[num_probes-1].id.length() + probes[num_probes-1].seq.length()); //probes
    used_memory += probes.size() * extra_memory_per_probe;

    if (use_hash) {
        if (verbose) cout << "building hash table...";
        double NN;
        if (num_mismatch > 0 && hash_1bp_mismatch)
            NN = num_probes * probe_len * 3 * 2.0;
        else
            NN = num_probes * 2.0;

        int logNN = (int)ceil(log(NN) / log(double(2.0)));
        if (logNN >= 32) panic("too many probes");
        nn = 1 << logNN;
        used_memory += (size_t)nn * sizeof(uint);
        if (verbose) cout << "estimated memory usage: " << (int)(used_memory >> 20) << "MB.\n";
        if (used_memory > available_memory) {
            panic("not enough memory.\n");
        }
        if (hash_table != NULL) {
            delete hash_table;
            hash_table = NULL;
        }
        hash_table = new uint[nn];
    rebuild:
        hash_magic = rand();
        for (i = 0; i < nn; i++) {
            hash_table[i] = uint(-1);
        }
        for (i = 0; i < num_probes; i++) {
            if (verbose && i % (num_probes / 20) == 0)  cout << '.'<< flush;
            int inserted_own = false;
            if (!probes[i].valid) continue;
            int64 key = probes[i].key;
            if (num_mismatch == 0 || !hash_1bp_mismatch) {
                if (!hash_insert_probe(i, 0, (uint)(key & 3))) {
                    if (verbose) cout << " circle found, rebuild" << endl;
                    goto rebuild;
                }
                continue;
            }
            for (j = 0; j < probe_len; j++) {
                for (k = 0; k < 4; k++) {
                    int64 temp = get_probe(encode_probe_index(i, j, k));
                    if (temp == key) {
                        if (inserted_own) continue;
                        inserted_own = true;
                    }
                    if (!hash_insert_probe(i, j, k)) {
                        if (verbose) cout << " circle found, rebuild" << endl;
                        goto rebuild;
                    }
                }
            }
        }
        if (verbose) {
            if (verbose) cout << num_probes << " probes inserted, size:" << sizeof(uint) * nn << " bytes" << endl;
#ifdef __DEBUG
            if (verbose) cout << "average insertion time:" << (double)total_jump1 / total_insertion << endl;
            if (verbose) cout << "maximum insertion time:" << max_jump1 << endl;
#endif
            if (verbose) cout << "testing hash table...";
        }
        for (i = 0; i < num_probes; i++) {
            if (!probes[i].valid) continue;
            int64 temp = probes[i].key;
            vector<uint> indexes;
            bool in = false;
            if (hash_detect_probe(temp, indexes) != (uint)(-1)) {
                for (j = 0; j < (int)indexes.size(); j++)
                    if (decode_probe_index(indexes[j]) == uint(i)) {
                        in = true;
                        break;
                    }
            }
            if (!in) panic("error");
            if (num_mismatch > 0 && hash_1bp_mismatch) {
                uint j = rand()%probe_len;
                uint k = rand()%4;
                temp = change_probe(temp, j, k);
                vector<uint> indexes;
                bool in = false;
                if (hash_detect_probe(temp, indexes) != (uint)(-1)) {
                    for (j = 0; j < (int)indexes.size(); j++)
                        if (decode_probe_index(indexes[j]) == uint(i)) {
                            in = true;
                            break;
                        }
                }
                if (!in) panic("error");
            }
            if (verbose && (i % (num_probes/20) == 0)) cout << "."<< flush;
        }
        if (verbose) {
            if (verbose) cout << "passed." << endl;
#ifdef __DEBUG
            if (verbose) cout << "average query time:" << (double)total_jump2 / total_detection << endl;
            if (verbose) cout << "maximum query time:" << max_jump2 << endl;
#endif
        }
    } else {
    //compute num_part
        int num_part = num_mismatch + 1;
        int effect_len = probe_len - num_insdel;
        while (true) {
            if (num_part == effect_len) break;
            int num_char_per_part = effect_len / num_part;
            int effect_char = num_char_per_part * (num_part - num_mismatch);
            if (effect_char*2 >= (int)(log((double)(num_probes))/log((double)2.0)))
                break;
            num_part++;
        }

        if(limit_num_part > 0 && num_part > limit_num_part) num_part = limit_num_part;

        if (verbose) cout << "Split probe into " << num_part << " parts." << endl;

        int num_lists = 1;
        for (i = 0; i < num_mismatch; i++)
            num_lists *= (num_part - i);
        for (i = 0; i < num_mismatch; i++)
            num_lists /= (i + 1);

        vector<vector<int64> > mask_sets;
        mask_sets.clear();

    //build probe lists
        int part_change[max_mismatch];

        for (j = 0; j < num_mismatch; j++)
            part_change[j] = j;

        while (true) {
            int64 m = 0;
            for (j = 0; j < num_part; j++) {
                for (k = 0; k < num_mismatch; k++)
                    if (part_change[k] == j) break;
                if (k < num_mismatch) continue;
                int i1 = effect_len * j / num_part + num_insdel;
                int i2 = effect_len * (j + 1) / num_part + num_insdel;
                for (k = i1; k < i2; k++)
                    m |= (((int64)(1) << (k*2)) | ((int64)(1) << (k*2+1)));
            }

            vector<int64> masks;
            masks.push_back(m);
            mask_sets.push_back(masks);

            int cur_pos = num_mismatch - 1;
            if (cur_pos == -1) break;
            part_change[cur_pos] = part_change[cur_pos] + 1;
            while (part_change[cur_pos] == num_part - num_mismatch + cur_pos + 1) {
                cur_pos--;
                if (cur_pos == -1) break;
                part_change[cur_pos] = part_change[cur_pos] + 1;
                for (j = cur_pos + 1; j < num_mismatch; j++)
                    part_change[j] = part_change[cur_pos] + j - cur_pos;
            }
            if (cur_pos == -1) break;
        }

        __ASSERT(num_lists == (int)mask_sets.size(), "internal error: wrong num_lists.\n");

        for (i = 0; i < num_lists; i++) {
            int64 mask = mask_sets[i][0];
            __ASSERT((mask & ((1 << num_insdel) - 1)) == 0, "internal error: wrong mask.\n");
            _insdel_masks(mask_sets[i], mask, num_insdel);
        }

        int num_mask_sets = 0;
        int max_num_mask_sets = 0;
        for (i = 0; i < num_lists; i++) {
            num_mask_sets += (int)mask_sets[i].size();
            max_num_mask_sets = max(max_num_mask_sets, (int)mask_sets[i].size());
        }
        if (verbose) cout << "total " << num_mask_sets << " copies of probes.\n";

        used_memory += num_lists * 65536 * 4 * 8; //mask0_table
        used_memory += (size_t)num_probes * num_mask_sets * 4; //index_list
        if (store_key) used_memory += (size_t)num_probes * num_mask_sets * 8; //key_list
        used_memory += (size_t)num_probes * max_num_mask_sets * 16;
        if (fast_index) used_memory += (size_t)((size_t)num_probes * num_mask_sets * 4 / fast_index_fraction); //fast_indexes
        if (verbose) cout << "estimated memory usage: " << (int)(used_memory >> 20) << "MB.\n";
        if (used_memory > available_memory) {
            panic("not enough memory.\n");
        }

        if (verbose) cout << "Building " << num_lists << " probe lists";
        probe_lists.clear();
        for (i = 0; i < num_lists; i++) {
            if (verbose) cout << "." << flush;
            probe_list *temp_probe_list = new probe_list();
            temp_probe_list->pm = this;
            temp_probe_list->set_mask(mask_sets[i]);
            probe_lists.push_back(temp_probe_list);
        }

        if (verbose) cout << (int)probe_lists.size() << " probe lists created." << endl;
    }

//test speed
    clock_t clock1 = clock();
    i = 0;
    while(true) {
        i++;
        int64 key = (int64)rand() * (int64)rand() * (int64)rand() * (int64)rand();
        correct_key(key);
        vector<uint> indexes;
        search_key(key, indexes);
        clock_t clock2 = clock();
        if (difftime_clock(clock2, clock1) >= 1) break;
    }
    clock_t clock2 = clock();
    if (verbose) cout << "estimated search speed is " << ((search_both_strands && !duplicate_probes)?(i/difftime_clock(clock2, clock1)/2.0):(i/difftime_clock(clock2, clock1))) << " bps/sec.\n";

    return true;
}

inline bool probe_match::initialize(const string &fa_file_name){
    if (!file_exists(fa_file_name)) panic("probe file doesn't exist.\n");
    probe_input_file_name = fa_file_name;
    if (is_fasta_file(fa_file_name)) {
        fasta myfasta;
        myfasta.read_from_file(fa_file_name, false);
        return initialize(myfasta.sequences, myfasta.tags);
    } else if (is_dna_file(fa_file_name)) {
        vector<string> probes;
        probes.clear();
        if (read_dna_from_file(fa_file_name, probes)) {
            return initialize(probes);
        } else {
            panic("probe file is in bad format.\n");
        }
    } else {
        panic("probe file is in bad format.\n");
    }
    return false;
}

inline bool probe_match::initialize(vector<string> &probe_sequences, vector<string> &probe_tags){
    ofstream file2, file3;
    if (output_filtered_probes) {
        file2.open((probe_input_file_name+".filtered").c_str());
        file3.open((probe_input_file_name+".removed").c_str());
    }
    set<string> filter_selected_probe_ids;
    filter_selected_probe_ids.clear();
    if (filter_selected_probes) {
        if (verbose) cout << "reading probe filtering list.\n";
        vector<string> temp_probes;
        if (!read_dna_from_file(filter_selected_probes_filename, temp_probes)) panic("probe filtering file is in bad format.\n");
        for (int i = 0; i < (int)temp_probes.size(); i++) filter_selected_probe_ids.insert(temp_probes[i]);
    }

    if (verbose) cout << "analysing probes...";
    __ASSERT(probe_sequences.size() > 0 && probe_sequences.size() == probe_tags.size(), "internal error, probe_sequences has bad size.\n");
    int min_len = 1000000000, max_len = -1;
    for (int i = 0; i < (int)probe_sequences.size(); i++) {
        probe_sequences[i] = trim_space(toupper(probe_sequences[i]));
        probe_tags[i] = trim_space(probe_tags[i]);
        min_len = min(min_len, (int)probe_sequences[i].length());
        max_len = max(max_len, (int)probe_sequences[i].length());
    }
    probe_len = min_len;
    if (probe_len < 2) probe_len = 2;
    if (probe_len > MAX_PROBE_LEN - num_insdel - 1) probe_len = MAX_PROBE_LEN - num_insdel - 1;
    if (cut_start > 0) probe_len = min(cut_end - cut_start + 1, probe_len);
    if (verbose) cout << "totally " << (int)probe_sequences.size() << " probes, minimum length = " << min_len << ", maximum length = " << max_len << ", set internal key length = " << probe_len << endl;

    if (verbose) cout << "importing probes...";
    int num_probe_skipped = 0;
    for (int i = 0; i < (int)probe_sequences.size(); i++) {
        if (verbose && i % 200000 == 0) cout << "." << flush;
        string probe_id = trim_space(probe_tags[i]);
        probe_info temp_probe_info;
        temp_probe_info.valid = true;
        temp_probe_info.rematch = false;
        temp_probe_info.key = 0;
        temp_probe_info.id = probe_id;
        temp_probe_info.repeat = -1;
        string probe = trim_space(toupper(probe_sequences[i]));
        if (cut_start > 0) {
            probe = probe.substr(cut_start - 1, cut_end - cut_start + 1);
        }

        if (probe.length() == 0) {
            if (verbose) cout << "bad probe length" << endl;
            break;
        }
        temp_probe_info.seq = probe;
        if (filter_selected_probes) {
            if (filter_selected_probe_ids.count(probe_id)) {
                if (output_filtered_probes) file3 << ">" << probe_id << endl << probe << endl;
                temp_probe_info.valid = false;
            }
        }
        if ((int)probe.length() != probe_len) {
            if ((int)probe.length() > probe_len) {
                probe = probe.substr(0, probe_len);
                temp_probe_info.rematch = true;
            } else if ((int)probe.length() < probe_len && (match_shorter_probes)) {
                temp_probe_info.rematch = true;
            } else {
                if (verbose && num_probe_skipped < 10) cout << "Error reading probe " << probe_id << ":" << temp_probe_info.seq << ". probably too short, skipped." << endl;
                num_probe_skipped++;
                if (output_filtered_probes) file3 << ">" << probe_id << endl << temp_probe_info.seq << endl;
                temp_probe_info.valid = false;
            }
        }
        probe = toupper(probe);
        if (filter_low_quality_probes) {
            if (probe.find("AAAAA") != string::npos || probe.find("CCCCC") != string::npos || probe.find("TTTTT") != string::npos || probe.find("GGGGG") != string::npos) {
                if (output_filtered_probes) file3 << ">" << probe_id << endl << temp_probe_info.seq << endl;
                temp_probe_info.valid = false;
            }
        }
        if (!temp_probe_info.valid) {
            probes.push_back(temp_probe_info);
            continue;
        }
        if (match_shorter_probes && (int)probe.length() < probe_len) {
            fix_short_probe(probe, num_mismatch);
            temp_probe_info.rematch = true;
        }
        if (match_N) {
            if (fix_N_probe(probe, num_mismatch)) {
                temp_probe_info.rematch = true;
            }
        }
        __ASSERT((int)probe.length() == probe_len, "internal error, bad probe.\n");
        temp_probe_info.key = tonumber(probe);
        if (temp_probe_info.key != -1) {
            temp_probe_info.valid = true;
            if (output_filtered_probes) file2 << ">" << probe_id << endl << temp_probe_info.seq << endl;
        } else {
            if (verbose && num_probe_skipped < 10) cout << "bad probe charactor in probe " << probe_id << ":" << temp_probe_info.seq << ". skipped\n";
            num_probe_skipped++;
            if (output_filtered_probes) file3 << ">" << probe_id << endl << temp_probe_info.seq << endl;
            temp_probe_info.valid = false;
        }
        probes.push_back(temp_probe_info);
    }
    if (verbose && num_probe_skipped > 0) cout << num_probe_skipped << " probes skipped.\n";
    num_probes = (int)probes.size();
    if (output_filtered_probes) {
        file2.close();
        file3.close();
    }

    if (cluster_identical_probes) {
        if (verbose) cout << "detecting repeated probes...";
        int num_repeat_probes = 0;
        vector<pair<string, int> > temp_probes;
        temp_probes.resize(num_probes);
        for (int i = 0; i < num_probes; i++) temp_probes[i] = pair<string, int>(probes[i].seq, i);
        sort(temp_probes.begin(), temp_probes.end());
        int first_probe_id = -1;
        for (int i = 0; i < num_probes; i++) {
            if (first_probe_id < 0 || temp_probes[i].first != probes[first_probe_id].seq) {
                first_probe_id = temp_probes[i].second;
                probes[temp_probes[i].second].repeat = -1;
            } else {
                probes[temp_probes[i].second].repeat = first_probe_id;
                probes[temp_probes[i].second].valid = false;
                num_repeat_probes++;
            }
        }
        if (verbose) cout << "found " << num_repeat_probes << " repeated probes.\n";
    }

    return initialize();
}

inline bool probe_match::search_key(const int64 key, vector<uint> &indexes){
    vector<uint> temp1, temp2;
    temp2.clear();
    int i, j;

    if (use_hash) {
        int ll;
        if (hash_detect_probe(key, temp1) != (uint)(-1)) {
            for (ll = 0; ll < (int)temp1.size(); ll++) {
                temp2.push_back(decode_probe_index(temp1[ll]));
            }
        }

        if (num_mismatch >= 1 && !hash_1bp_mismatch || num_mismatch >= 2) {
            int num_change = hash_1bp_mismatch ? num_mismatch - 1 : num_mismatch;
            int pos_change[max_mismatch - 1];
            int char_change[max_mismatch - 1];
            int jj;
            for (jj = 0; jj < num_change; jj++) {
                pos_change[jj] = 0;
                char_change[jj] = 0;
            }
            while (true) {
                int64 temp = key;
                for (jj = 0; jj < num_change; jj++)
                    temp = change_probe(temp, pos_change[jj], char_change[jj]);
                if (hash_detect_probe(temp, temp1) != (uint)(-1)) {
                    for (ll = 0; ll < (int)temp1.size(); ll++) {
                        temp2.push_back(decode_probe_index(temp1[ll]));
                    }
                }
                int cur_pos = num_change - 1;
                char_change[cur_pos] = char_change[cur_pos] + 1;
                while (char_change[cur_pos] == 4) {
                    char_change[cur_pos] = 0;
                    cur_pos--;
                    if (cur_pos == -1) break;
                    char_change[cur_pos] = char_change[cur_pos] + 1;
                }
                if (cur_pos == -1) {
                    cur_pos = num_change - 1;
                    pos_change[cur_pos] = pos_change[cur_pos] + 1;
                    while (pos_change[cur_pos] == probe_len) {
                        cur_pos--;
                        if (cur_pos == -1) break;
                        pos_change[cur_pos] = pos_change[cur_pos] + 1;
                        for (jj = cur_pos + 1; jj < num_change; jj++)
                            pos_change[jj] = pos_change[cur_pos];
                    }
                }
                if (cur_pos == -1) break;
            }
        }
    } else {
        for (i = 0; i < (int)probe_lists.size(); i++) {
            temp1.clear();
            if (probe_lists[i]->search_probe(key, temp1)){
                for (j = 0; j < (int)temp1.size(); j++) {
                    temp2.push_back(temp1[j]);
                }
            }
        }
    }

    indexes.clear();
    sort(temp2.begin(), temp2.end());
    for (j = 0; j < (int)temp2.size(); j++) {
        if (j == 0 || (temp2[j] != temp2[j - 1])) {
            indexes.push_back(temp2[j]);
        }
    }
    return (indexes.size() > 0);
}

inline void probe_match::search_results_prehandler(vector<search_result_struct> &results, search_result_handler handler) {
    int i;
    vector<search_result_struct> temp_results;
    temp_results.clear();

    for (i = 0; i < (int)results.size(); i++) {
        if (!results[i].reported) {
            temp_results.push_back(results[i]);
        }
    }

    handler(temp_results);
}

inline bool probe_match::search_file(const string &fa_file_name,  search_result_handler handler){
    if (!file_exists(fa_file_name)) panic("transcripts file doesn't exist.\n");
    int i, j, k;
    clock_t clock1 = clock();
    if (verbose) cout << "detecting probes" << endl;
    ifstream file2(fa_file_name.c_str());

    transcripts.clear();
    int current_transcript = 0;
    string transcript_id = "";
    string next_trans_id = "";
    int64 basepair_processed = 0;
    string line = "";
    int64 key = 0;
    int pos = zero_indexed?0:1;
    int last_N = pos-1;
    int current_pos = 0;
    int key_len = probe_len + num_insdel;
    int len_buf = 512;
    vector<search_result_struct> search_results;
    search_results.clear();
    int num_bad_chr_skipped = 0;
     while (true) {
        while (current_pos > (int)line.length() - len_buf) {
            if (next_trans_id != "") break;
            string templine;
            if (getline(file2, templine)) {
                if (templine[0] == '>') {
                    next_trans_id = templine.substr(1, string::npos);
                    break;
                } else {
                    line = line + templine;
                }
            } else break;
        }
        if (current_pos >= (int)line.length() + num_insdel * 2) {
            if (search_results.size() > 0) search_results_prehandler(search_results, handler);
            search_results.clear();

            if (next_trans_id == "") break;

            transcript_id = next_trans_id;
            transcripts.push_back(transcript_id);
            next_trans_id = "";
            current_transcript++;
//            if (verbose && current_transcript > 1) cout << endl;
//            if (verbose) cout << "processing transcript " << transcript_id;
            pos = zero_indexed?0:1;
            last_N = pos-1;
            current_pos = 0;
            line = "";
            key = 0;
            continue;
        }
        char ch;
        if (current_pos < (int)line.length()) ch = line[current_pos];
        else ch = random_NT();

        if (ch >= 'a' && ch <= 'z') {
            if (search_repeats) ch = (char)toupper(ch);
            else ch = 'N';
        }
        if (ch == 'N') {
            last_N = pos;
            ch = 'A';
        }
        for (i = 0; i < 4; i++) {
            if (ch == DNA_C[i]) break;
        }

        if (i >= 4) {
            if (verbose && num_bad_chr_skipped < 10) cout << "Bad charactor " << ch << " found when processing transcript " << transcript_id << ". Skipped." << endl;
            num_bad_chr_skipped++;
            last_N = pos;
            ch = 'A';
            i = 0;
        }
        key <<= 2;
        correct_key(key, key_len);
        key |= i;
        current_pos++;
        while (current_pos > len_buf + 1000) {
            current_pos -= 1000;
            line = line.substr(1000, string::npos);
        }
        pos++;
        if (verbose && pos % (1000000) == 0) cout << "."<< flush;
        basepair_processed++;
        if (pos - last_N <= key_len) continue;
        vector<uint> indexes;
        indexes.clear();
        vector<search_result_struct> results;
        results.clear();
        if (search_key(key, indexes)) {
            for (j = 0; j < (int)indexes.size(); j++) {
                search_result_struct temp_result;
                temp_result.trans_key_len = key_len;
                temp_result.probe_index = indexes[j];
                if (probes[indexes[j]].rematch || current_pos > (int)line.length()) {
                    string probe_seq = probes[indexes[j]].seq;
                    int temp_len = (int)probe_seq.length();
                    int start = current_pos - key_len;
                    int end = min((int)line.length(), current_pos - key_len + temp_len + num_insdel);
                    if (duplicate_probes && (int)indexes[j] >= num_probes / 2) {
                        probe_seq = get_reverse2(probe_seq);
                    }
                    __ASSERT(start >= 0 && start < end && end <= (int)line.length(), "internal error: bad trans_seq\n");
                    temp_result.trans_seq = toupper(line.substr(start, end - start));
                    temp_result.trans_key_len = end - start;
                    temp_result.num_mismatch = mismatch(temp_result.trans_seq, probe_seq, num_mismatch, num_insdel, temp_result.trans_key_len);
                    temp_result.trans_seq = temp_result.trans_seq.substr(0, temp_result.trans_key_len);
                    temp_result.trans_key = key;
                } else {
                    temp_result.num_mismatch = mismatch(key, probes[indexes[j]].key, num_mismatch, num_insdel, temp_result.trans_key_len);
                    __ASSERT(temp_result.num_mismatch <= num_mismatch, "internal errro: num_mismatch too big\n");
                    temp_result.trans_key = (key >> (2 * (key_len - temp_result.trans_key_len)));
                    temp_result.trans_seq = tostring(temp_result.trans_key, temp_result.trans_key_len);
                }
                if (temp_result.num_mismatch > num_mismatch) continue;
                temp_result.trans_id = current_transcript - 1;
                temp_result.trans_coord = pos - key_len;
                temp_result.reverse_strand = false;
                if (duplicate_probes && temp_result.probe_index >= num_probes / 2) {
                    temp_result.reverse_strand = true;
                    temp_result.probe_index -= num_probes / 2;
                }

                temp_result.reported = false;
                results.push_back(temp_result);
            }
        }
        indexes.clear();
        if (search_both_strands && (!duplicate_probes) && search_key(get_reverse(key, key_len), indexes)) {
            for (j = 0; j < (int)indexes.size(); j++) {
                search_result_struct temp_result;
                temp_result.trans_key_len = key_len;
                temp_result.probe_index = indexes[j];
                if (probes[indexes[j]].rematch || current_pos > (int)line.length()) {
                    string probe_seq = probes[indexes[j]].seq;
                    int temp_len = (int)probe_seq.length();
                    int start = max(0, current_pos - temp_len - num_insdel);
                    int end = min((int)line.length(), current_pos);
                    probe_seq = get_reverse2(probe_seq);
                    __ASSERT(start >= 0 && start < end && end <= (int)line.length(), "internal error: bad trans_seq\n");
                    temp_result.trans_seq = toupper(line.substr(start, end - start));
                    temp_result.trans_key_len = end - start;
                    temp_result.num_mismatch = mismatch(temp_result.trans_seq, probe_seq, num_mismatch, num_insdel, temp_result.trans_key_len);
                    temp_result.trans_seq = temp_result.trans_seq.substr(0, temp_result.trans_key_len);
                    temp_result.trans_key = key;
                } else {
                    temp_result.num_mismatch = mismatch(get_reverse(key, key_len), probes[indexes[j]].key, num_mismatch, num_insdel, temp_result.trans_key_len);
                    __ASSERT(temp_result.num_mismatch <= num_mismatch, "internal errro: num_mismatch too big\n");
                    temp_result.trans_key = (key >> (2 * (key_len - temp_result.trans_key_len)));
                    temp_result.trans_seq = tostring(temp_result.trans_key, temp_result.trans_key_len);
                }
                if (temp_result.num_mismatch > num_mismatch) continue;
                temp_result.trans_id = current_transcript - 1;
                temp_result.trans_coord = pos - key_len;
                if (probes[indexes[j]].rematch) temp_result.trans_coord = pos - (int)probes[indexes[j]].seq.length() - num_insdel;
                temp_result.reverse_strand = true;

                temp_result.reported = false;
                results.push_back(temp_result);
            }
        }

        if (filter_results) {
            vector<search_result_struct> temp_results;
            temp_results.clear();
            for (j = 0; j < (int)search_results.size(); j++) {
                bool remain = true;
                for (k = 0; k < (int)results.size(); k++) {
                    if (filter_results && search_results[j].probe_index == results[k].probe_index && search_results[j].num_mismatch > results[k].num_mismatch) {
                        remain = false;
                        break;
                    }
                }
                if (remain && !search_results[j].reported) temp_results.push_back(search_results[j]);
            }
            if (temp_results.size() > 0) search_results_prehandler(temp_results, handler);
            temp_results.clear();
            for (j = 0; j < (int)results.size(); j++) {
                for (k = 0; k < (int)search_results.size(); k++) {
                    if (filter_results && search_results[k].probe_index == results[j].probe_index && search_results[k].num_mismatch <= results[j].num_mismatch) {
                        results[j].reported = true;
                        break;
                    }
                }
            }
            search_results.clear();
            search_results = results;
        } else {
            handler(results);
        }
        results.clear();
    }

    file2.close();
    clock_t clock2 = clock();
    if (verbose && num_bad_chr_skipped > 0) cout << num_bad_chr_skipped << " bad characters skipped.\n";
    if (verbose) cout << endl;
#ifdef __DEBUG
    if (verbose && (!use_hash)) cout << "average search steps:" << (double)num_total_search_step / num_search << " maximum search steps:" << num_max_search_step << endl;
#endif
    if (verbose) cout << basepair_processed << " base pairs processed. average search speed: " << basepair_processed / difftime_clock(clock2, clock1) << " bps/sec.\n";
//    if (verbose) cout << "memory used: " << (int)(used_memory >> 20) << "MB.\n";
    return true;
}

#endif//PROBE_MATCH_H


probe_match myprobe_match;
ofstream file;
bool statistics_only = true;
bool output_probe_without_match = true;
bool output_alignment = false;
int num_top_matches = 2;
int num_probes;
bool do_eland = true;
int eland_version = 2;

class mini_search_result_struct{
public:
    int trans_id;
    int trans_coord;
    bool reverse_strand;
    uchar num_mismatch;
};

int* match_counts;
vector<vector<mini_search_result_struct> > top_matches;

void file_write_handler(vector<search_result_struct> &results) {
    int i, j;
    for (i = 0; i < (int)results.size(); i++) {
        __ASSERT(!results[i].reported, "internal error: reported.\n");
        if (statistics_only) {
            __ASSERT(results[i].probe_index >= 0 && results[i].probe_index < num_probes, "internal error: wrong probe_index.\n");
            __ASSERT(results[i].num_mismatch >= 0 && results[i].num_mismatch <= myprobe_match.num_mismatch, "internal error: wrong num_mismatch.\n");
            match_counts[results[i].probe_index * (myprobe_match.num_mismatch + 1) + results[i].num_mismatch]++;
            j = 0;
            for (j = 0; j < (int)top_matches[results[i].probe_index].size(); j++) {
                if (results[i].num_mismatch < top_matches[results[i].probe_index][j].num_mismatch) {
                    break;
                }
            }
            if (j < num_top_matches) {
                mini_search_result_struct result;
                result.trans_id = results[i].trans_id;
                result.trans_coord = results[i].trans_coord;
                result.reverse_strand = results[i].reverse_strand;
                result.num_mismatch = (uchar)results[i].num_mismatch;
                top_matches[results[i].probe_index].insert(top_matches[results[i].probe_index].begin() + j, result);
            }

            if ((int)top_matches[results[i].probe_index].size() > num_top_matches) {
                top_matches[results[i].probe_index].erase(top_matches[results[i].probe_index].begin() + num_top_matches);
                __ASSERT((int)top_matches[results[i].probe_index].size() == num_top_matches, "internal error: wrong num_top_matches.\n");
            }
        } else {
            if (results[i].reverse_strand) results[i].trans_seq = get_reverse2(results[i].trans_seq);
            string trans_seq = results[i].trans_seq, probe_seq = myprobe_match.probes[results[i].probe_index].seq;
            if (output_alignment){
                local_alignment(trans_seq, probe_seq, myprobe_match.num_insdel);
            }
            file << myprobe_match.transcripts[results[i].trans_id] << "\t" << results[i].trans_coord << "\t" << trans_seq;
            file << "\t" << myprobe_match.probes[results[i].probe_index].id;
            file << "\t" << probe_seq << "\t" << results[i].num_mismatch;
            if (myprobe_match.search_both_strands) {
                if (results[i].reverse_strand) file << "\t-";
                else file << "\t+";
            }
            file << endl;
        }
    }
}

void do_statistics(){
    for (int i = 0; i < num_probes; i++) {
        if (myprobe_match.probes[i].repeat >= 0) {
            int mirror_probe = myprobe_match.probes[i].repeat;
            for (int j = 0; j <= myprobe_match.num_mismatch; j++) {
                match_counts[i * (myprobe_match.num_mismatch + 1) + j] = match_counts[mirror_probe * (myprobe_match.num_mismatch + 1) + j];
            }
            top_matches[i] = top_matches[mirror_probe];
        }
    }

    if (do_eland) {
        for (int i = 0; i < num_probes; i++) {
            if (eland_version > 1) file << myprobe_match.probes[i].id << "\t";
            file << myprobe_match.probes[i].seq;
            int top_target = -1;
            if (top_matches[i].size() > 0) top_target = top_matches[i][0].num_mismatch;
            bool unique_target = false;
            if (top_matches[i].size() == 1 || top_matches[i].size() > 1 && top_matches[i][0].num_mismatch < top_matches[i][1].num_mismatch) unique_target = true;
            int* target_counts = match_counts + i * (myprobe_match.num_mismatch + 1);
            if (eland_version == 1) {
                file << "\t" << top_target + 1 << "\t";
                if (top_target >= 0) file << (int)target_counts[top_target]; else file << "0";
            } else if (eland_version == 2) {
                file << "\t";
                if (top_target >= 0) {
                    __ASSERT(target_counts[top_target] > 0, "internal error, target_counts[top_target] == 0");
                    if (target_counts[top_target] > 1) file << "R"; else file << "U";
                    file << top_target;
                } else file << "NM";
                for (int j = 0; j <= myprobe_match.num_mismatch; j++) file << "\t" << (int)target_counts[j];
            } else if (eland_version == 3){
                file << "\t";
                if (top_target < 0) file << "NM";
                else {
                    for (int j = 0; j <= myprobe_match.num_mismatch; j++) {
                        if (j > 0) file << ":";
                        file << (int)target_counts[j];
                    }
                }
            } else panic("bad eland_version");
            if (top_target >= 0) {
                if (unique_target && eland_version < 3) {
                    file << "\t" << myprobe_match.transcripts[top_matches[i][0].trans_id];
                    if (eland_version == 1) file << ":"; else file << "\t";
                    file << top_matches[i][0].trans_coord << "\t" << (top_matches[i][0].reverse_strand?"R":"F");
                } else if (eland_version == 3) {
                    file << "\t";
                    for (int j = 0; j < (int)top_matches[i].size(); j++) {
                        if (top_matches[i][j].num_mismatch > top_target + 1) break;
                        if (j > 0) file << ",";
                        file << myprobe_match.transcripts[top_matches[i][j].trans_id] << ":" << top_matches[i][j].trans_coord << (top_matches[i][j].reverse_strand?"R":"F") << (int)top_matches[i][j].num_mismatch;
                    }
                }
            }
            file << endl;
        }
    } else {
        int i, j;
        file << "probe_id";
        for (j = 0; j <= myprobe_match.num_mismatch; j++) {
            file << "\t#mismatch=" << j;
        }
        for (j = 0; j < num_top_matches; j++) {
            file << "\ttrans_id\tcoord\t#mismatch";
        }
        file << endl;
        for (i = 0; i < num_probes; i++) {
            int sum = 0;
            for (j = 0; j <= myprobe_match.num_mismatch; j++) {
                sum += match_counts[i * (myprobe_match.num_mismatch + 1) + j];
            }
            if (sum == 0 && !output_probe_without_match) continue;
            file << myprobe_match.probes[i].id;
            for (j = 0; j <= myprobe_match.num_mismatch; j++) {
                file << "\t" << match_counts[i * (myprobe_match.num_mismatch + 1) + j];
            }
            for (j = 0; j < (int)top_matches[i].size(); j++) {
                file << "\t" << myprobe_match.transcripts[top_matches[i][j].trans_id] << "\t" << top_matches[i][j].trans_coord << "\t" << (int)(top_matches[i][j].num_mismatch);
            }
            file << endl;
        }
    }
}

int main(int argc, char* argv[]){
    timer mytimer;
    int i, j;
    probe_len = -1;

    if (argc > 1 && argc < 5) {
        panic("not enough parameters.\n");
    } else if (argc <= 1) {
        printf("SeqMap: Short Sequence Mapping Tool\n\n");
        printf("Usage: seqmap <number of mismatches> <probe FASTA file name> <transcript FASTA file name> <output file name> [options]\n\n");
        printf("parameters/Options: (* for advanced users only)\n");
        printf("<number of mismatches>                         \tmaximum edit distance allowed\n");
        printf("<probe FASTA file name>                        \tprobe/tag/read sequences\n");
        printf("<transcript FASTA file name>                   \treference sequences\n");
        printf("<output file name>                             \tname of the output file\n");
        printf("[/eland[:style]]                               \toutput in eland style\n");
        printf("[/output_top_matches:num_top_matches]          \toutput the top\n\t\t\t\t\t\t \"num_top_matches\" targets\n");
        printf("[/forward_strand]                              \tsearch forward strand only\n");
        printf("[/allow_insdel:num_insdel]                     \tenable insertion and deletion\n");
        printf("[/cut:start,end]                               \tcut the probes\n");
        printf("[/match_shorter_probes]                        \tmatch shorter probes\n");
        printf("[/skip_N]                                      \tskip probes that have N or .\n");
        printf("[/no_repeats]                                  \tdo not search repeat regions\n");
        printf("[/silent]                                      \tdisable debug information\n");
        printf("[/available_memory:memory_size(in MB)]         \tdisable memory detection and\n\t\t\t\t\t\t set memory size manually\n");
        printf("[/zero_indexed]                                \toutput coordinates in\n\t\t\t\t\t\t 0-indexed manner\n");
        printf("[/output_all_matches]                          \toutput all matches\n");
        printf("[/exact_mismatch]                              \toutput targets with\n\t\t\t\t\t\t exact number of mismatches only\n");
        printf("*[/output_alignment]                            \toutput alignment\n");
        printf("*[/output_statistics]                          \toutput in probe order\n");
        printf("*[/do_not_output_probe_without_match]          \tignore non-matched probes\n");
        printf("*[/no_duplicate_probes]                        \tdo not duplicate probes for\n\t\t\t\t\t\t reverse strand search\n");
        printf("*[/use_hash]                                   \tuse hash if possible\n");
        printf("*[/no_hash_1bp_mismatch]                       \tdo not use hash for 1bp search\n");
        printf("*[/limit_num_part:max_num_part]                \tmaximum number of parts that\n\t\t\t\t\t\t a probe is splitted\n");
        printf("*[/interpolation_search]                       \tuse interpolation search\n\t\t\t\t\t\t ,not binary search\n");
        printf("*[/shift_mask]                                 \tmay reduce memory usage\n\t\t\t\t\t\t while increase running time\n");
        printf("*[/no_store_key]                               \tmay reduce memory usage\n\t\t\t\t\t\t while increase running time\n");
        printf("*[/no_fast_index]                              \tmay reduce memory usage\n\t\t\t\t\t\t while increase running time\n");
        printf("*[/fast_index_fraction:fraction]\n");
        printf("*[/no_filter_results]\n");
        printf("*[/filter_selected_probes:selected_probes_file]\tremove the probes in file\n");
        printf("*[/output_filtered_probes]                     \toutput the filtered probes\n");
        printf("*[/filter_low_quality_probes]                  \tfilter low quality probes\n");
        printf("\nVisit software homepage http://biogibbs.stanford.edu/~jiangh/SeqMap/ for more information. Bugs report to jiangh@stanford.edu.\n");
        return 1;
    }

    for (i = 5; i < argc; i++) {
        string option = argv[i];
        string command = "";
        string parameter = "";
        if (option[0] != '/') {
            panic(string("bad option ") + option);
        }
        size_t index = option.find (":");
        if (index != string::npos ) {
            command = option.substr(1, index - 1);
            parameter = option.substr(index + 1);
        } else {
            command = option.substr(1);
        }

        if (command == "forward_strand") {
            myprobe_match.search_both_strands = false;
        } else if (command == "silent") {
            myprobe_match.verbose = false;
        } else if (command == "exact_mismatch") {
            myprobe_match.exact_mismatch = true;
        } else if (command == "output_alignment") {
            output_alignment = true;
        } else if (command == "no_repeats") {
            myprobe_match.search_repeats = false;
        } else if (command == "interpolation_search") {
            myprobe_match.interpolation_search = true;
        } else if (command == "shift_mask") {
            myprobe_match.shift_mask = true;
        } else if (command == "no_store_key") {
            myprobe_match.store_key = false;
        } else if (command == "no_fast_index") {
            myprobe_match.fast_index = false;
        } else if (command == "limit_num_part") {
            myprobe_match.limit_num_part = str2int(parameter);
        } else if (command == "allow_insdel") {
            myprobe_match.num_insdel = str2int(parameter);
        } else if (command == "fast_index_fraction") {
            myprobe_match.fast_index_fraction = str2double(parameter);
        } else if (command == "cut") {
            vector<string> tokens = string_tokenize(parameter, ",");
            if (tokens.size() != 2) panic("bad cut parameters.\n");
            myprobe_match.cut_start = str2int(tokens[0]);
            myprobe_match.cut_end = str2int(tokens[1]);
            if (!(myprobe_match.cut_start >= 1 && myprobe_match.cut_end > myprobe_match.cut_start)) panic("bad cut parameters.\n");
        } else if (command == "no_duplicate_probes") {
            myprobe_match.duplicate_probes = false;
        } else if (command == "use_hash") {
            myprobe_match.use_hash = true;
        } else if (command == "no_filter_results") {
            myprobe_match.filter_results = false;
        } else if (command == "no_hash_1bp_mismatch") {
            myprobe_match.hash_1bp_mismatch = false;
        } else if (command == "output_statistics") {
            do_eland = false;
            num_top_matches = 2;
        } else if (command == "do_not_output_probe_without_match") {
            output_probe_without_match = false;
        } else if (command == "filter_selected_probes") {
            myprobe_match.filter_selected_probes = true;
            myprobe_match.filter_selected_probes_filename = parameter;
        } else if (command == "output_filtered_probes") {
            myprobe_match.output_filtered_probes = true;
        } else if (command == "filter_low_quality_probes") {
            myprobe_match.filter_low_quality_probes = true;
        } else if (command == "output_top_matches") {
            num_top_matches = str2int(parameter);
        } else if (command == "available_memory") {
            myprobe_match.available_memory = ((int64)(1<<20)) * str2int(parameter);
        } else if (command == "eland") {
            if (is_int(parameter)) eland_version = str2int(parameter);
            if (eland_version < 1 || eland_version > 3) eland_version = 3;
            if (eland_version == 3) num_top_matches = 10; else num_top_matches = 2;
        } else if (command == "match_shorter_probes") {
            myprobe_match.match_shorter_probes = true;
        } else if (command == "skip_N") {
            myprobe_match.match_N = false;
        } else if (command == "zero_indexed") {
            myprobe_match.zero_indexed = true;
        } else if (command == "output_all_matches") {
            statistics_only = false;
            myprobe_match.cluster_identical_probes = false;
        } else {
            panic(string("bad command ") + command);
        }
    }

    sscanf(argv[1], "%d", &myprobe_match.num_mismatch);
    if (myprobe_match.num_mismatch < 0) {
        myprobe_match.num_mismatch = - myprobe_match.num_mismatch;
        myprobe_match.exact_mismatch = true;
    }
    if (myprobe_match.num_mismatch < 0 || myprobe_match.num_mismatch > max_mismatch) {
        panic(string("ERROR: number of mismatches must be between 0 and ") + int2str(max_mismatch));
    }
    if (myprobe_match.num_insdel < 0 || myprobe_match.num_insdel > max_insdel) {
        panic(string("ERROR: number of insdels must be between 0 and ") + int2str(max_insdel));
    }
    if (myprobe_match.num_insdel > myprobe_match.num_mismatch) {
        panic("ERROR: number of insdels can not be greater than num of mismatches.\n\n");
    }

    if (myprobe_match.verbose) {
        cout << "Command line:";
        for (i = 0; i < argc; i++)
            cout << argv[i] << " ";
        cout << endl;
    }

    if (statistics_only) {
        myprobe_match.extra_memory_per_probe = (myprobe_match.num_mismatch + 1) * 4; //match_counts
        myprobe_match.extra_memory_per_probe += sizeof(vector<mini_search_result_struct>) + num_top_matches * sizeof(mini_search_result_struct); //top_matches;
    }

    myprobe_match.initialize(argv[2]);
    if (myprobe_match.duplicate_probes) num_probes = myprobe_match.num_probes / 2;
    else num_probes = myprobe_match.num_probes;
    match_counts = NULL;

    if (statistics_only) {
        match_counts = new int[num_probes * (myprobe_match.num_mismatch + 1)];
        top_matches.resize(num_probes);
        for (i = 0; i < num_probes; i++){
            for (j = 0; j <= myprobe_match.num_mismatch; j++) match_counts[i * (myprobe_match.num_mismatch + 1) + j] = 0;
            top_matches[i].clear();
        }
    }

    file.open(argv[4]);
    if (!statistics_only) {
        file << "trans_id\ttrans_coord\ttarget_seq\tprobe_id\tprobe_seq\tnum_mismatch";
        if (myprobe_match.search_both_strands) file << "\tstrand";
        file << endl;
    }
    myprobe_match.search_file(argv[3], file_write_handler);
    if (statistics_only) do_statistics();
    file.close();
    if (match_counts != NULL) delete[] match_counts;
}
