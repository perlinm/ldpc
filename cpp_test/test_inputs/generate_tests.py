import numpy as np

def random_binary_matrix(height: int = 10, width:int = 10, sparsity: float = 0.4, random_size: bool = False):

    if random_size:
        height = np.random.randint(1,height)
        width = np.random.randint(1, width)

    mat = np.zeros(height*width).astype(np.uint8)

    for i, _ in enumerate(mat):
        if np.random.random()<sparsity: mat[i] = 1

    return mat.reshape((height,width))

def to_csr(mat: np.ndarray):

    m,n = mat.shape

    csr_string ="{"
    for i in range(m):
        csr_string+="{"
        first = True
        for j in range(n):
            if mat[i,j] == 1:
                if first!=True: csr_string+=","
                first = False
                csr_string+=f"{j}"

        # if(!first): csr_string+="\b"   
        csr_string+="}"
        if i!=(m-1): csr_string+=","
    csr_string += "}"

    return csr_string


def add_rows_tests():

    output_file = open("gf2_add_test.csv", "w+")

    for j in range(100):
        for i in np.arange(0,10,0.5):
            height = np.random.randint(1,40)
            width = np.random.randint(1, 40)
            pcm = random_binary_matrix(height=height,width=width,sparsity=0.1*i)
            
            orig_pcm = to_csr(pcm)
            
            target_row = np.random.randint(height)
            add_row = np.random.randint(height)

            pcm[target_row] = (pcm[target_row] + pcm[add_row]) % 2

            final_pcm = to_csr(pcm)

            csv_string = f"{height};{width};{orig_pcm};{target_row};{add_row};{final_pcm}"
            print(csv_string, file=output_file)



if __name__ == "__main__":

    add_rows_tests()
    a = np.array([[1,0,0],[0,0,0],[0,0,0]])
    a_csr=to_csr(a)
    print(a_csr)