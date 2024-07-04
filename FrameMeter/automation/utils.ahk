get_array_index(array, value) {
	for i, v in array {
		if (v == value) {
			return i
		}
	}
	return -1
}