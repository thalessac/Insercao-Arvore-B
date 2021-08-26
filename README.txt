Código que implementa a função inserir em um arquivo de registros representando páginas de uma árvore B inicialmente vazia

A árvore armazena chaves do tipo inteiro, e possui ordem m=3, ou seja, 3 links por página, e portanto cada página pode ter até 2 chaves numeradas. Cada página é formada por um vetor de itens do tipo CHLINK que corresponde a um par de chave e link (direito exceto para o item 0, que é na verdade um link esquerdo). Um campo contador é incluído para armazenar a quantidade de chaves efetivamente armazenadas em cada página.

O arquivo da árvore B é formado por registros do tipo PAGINA, sendo um arquivo binário contendo registros de tamanho fixo.