<설명서>
2010100086 김학영

프로그램을 실행하면 메인 화면을 보실 수 있습니다.
메인 화면에 보이는 11개의 메뉴들 중, 실행하고 싶으신 메뉴 앞의 숫자를 입력하면 해당 기능을 실행할 수 있습니다.

프로그램을 이용하기 위해서는 가장 먼저 '0'번 기능을 통해 데이터베이스를 불러와야 합니다. 데이터베이스의 경로는 프로그램과 동일해야 하며, 각각 user.txt, friend.txt, word.txt의 이름의 텍스트 파일이어야 합니다. 'LOADING DB'라는 글자가 사라지고 모든 사용자, 친구관계, 트윗 수가 뜨면 데이터베이스를 정상적으로 불러온 것이므로 이제부터 모든 다른 기능들을 정상적으로 사용할 수 있습니다.

기본적으로 메뉴가 완전히 실행되고 나면 'Press 'Enter' to return to main menu'라는 문구가 뜹니다. 이 문구가 뜨지 않았다면 아직 데이터를 처리중이라는 의미이니 조금만 더 기다려주시기 바랍니다. 문구가 뜬 것을 확인하셨다면 아무 버튼이나 누르시면 다시 메인 화면으로 돌아가셔서 원하는 메뉴를 고르실 수 있습니다.

프로그램을 종료하시려면 '99'를 입력하시면 됩니다.
감사합니다.




<리포트>

  기능구현을 위해 크게 4개의 구조체(user, tweetadj, adj, stack)와 1개의 배열(hash)를 사용했습니다. 데이터베이스에 포함된 사용자, 트윗, 친구관계 수에 구애받지 않도록 abstract data structure를 사용하였으며, hash 배열은 199라는 임의의 hash에 사용되는 값을 정해놓았기 때문에 굳이 abstract일 필요가 없다고 생각되어 199 크기의 배열로 지정하였습니다.
  user 구조체는 circular doubly linked list를 이용하여 user 검색에 있어서 어느 user의 포인터에서 검색을 시작하든 큰 무리가 없도록 하였습니다. user간의 친구관계를 나타내는 adj 구조체는 각 user마다 부여되어  id, 또는 master로 설정되어, 누가 누구의 친구인지 방향성을 나타낼 수 있도록 하였으며, 마찬가지로 포인터 이동이 일반 linked list보다 용이하도록 doubly linked list를 선택했습니다. 친구관계의 경우 해당 user의 친구를 한 번 훑기만 하면 모든 기능을 구현하는데에 무리가 없었기 때문에 굳이 circular한 특징을 두지는 않았습니다. tweetadj 구조체는 각 user가 작성한 tweet을 위한 구조체로, adj와 마찬가지로 각 user마다 부여된 doubly linked list입니다. tweet은 추가로 문자열을 통해 해쉬값을 추출하여 그 값을 기준으로 hash 배열에 해당 tweetadj의 포인터를 지정했습니다. collision이 일어나는 경우가 많을 것이므로(같은 단어를 tweet하는 경우 같은 해쉬값을 가지므로) 그 경우 collision이 일어난 tweetadj끼리를 연결하는 hashprev, hashnext를 추가로 두어 (2)번 기능을 더 효율적으로 구현하려 했습니다. 마지막으로 stack 구조체는 (8)번 기능에서의 Strongly Connected Components 들을 찾기 위해 사용된 Depth First Search 알고리즘과 (9)번 기능에서의 최단경로를 찾기 위한 Dijkstra 알고리즘에 쓰기 위한 구조입니다. DFS는 기본적으로 스택을 사용하기 때문에 pop과 push를 이용하여 stack 구조체를 통해 스택을 구현하였고, Dijkstra는 큐를 사용하지만, enqueue에서 최소값이 가장 먼저 출력되도록 큐에 입력되게 하여 굳이 같은 구조를 가진 queue 구조체를 따로 만들지 않고, stack 구조체를 이용해 최소우선큐를 구현했습니다.
  개선할 점은 다음과 같습니다.
	(0)번 기능에는 데이터베이스의 경로를 설정할 수 있게 하면 프로그램의 데이터베이스에의 접근성을 높일 수 있습니다.
	(2)번 기능에는 자주 등장하는 단어를 최대로 분산시키도록 stringhash()명령을 수정하여 collision을 최소화하면 더 빠른 결과를 출력할 수 있습니다.
	(7)번 기능에는 현재 구현된 (6)번 기능과 같이 실제 txt 데이터베이스에도 삭제가 이루어지게 하여 데이터베이스의 수정이 용이하도록 할 수 있습니다.
	(9)번 기능에는 목적지의 user id를 입력받을 수 있게 하여 특정 id 끼리만의 최단 경로를 볼 수 있도록 기능을 추가할 수 있습니다.
	프로그램의 언어를 선택하는 기능을 추가하여 한글 버전을 추가할 수 있습니다.
	

• Submit a github account (10)
	10.

• Commit source code displaying menu (10)
	10.

• Commit the first draft of manual (10)
	10.

• Read data files (20)
	20.

• Statistics (20)
	20. user수, tweet수, friend record수를 global 변수로 지정해놓고, 친구의 수를 user 구조체의 weight에 저장해놓아, 0번 기능을 수행함과 동시에 계산을 마치므로, 해당 기능이 실행 될 때 실질적으로 계산하는 것은 사용자 간 최대/최소 tweet수 뿐입니다. 따라서 O(n)의 시간복잡도를 갖습니다.

• Top 5 most tweeted words (10)
	8. 해쉬 알고리즘을 이용하여 트윗내용을 hash배열과 linked list구조를 이용하여 메모리에 저장해놓았습니다. 따라서 같은 내용의 트윗은 같은 hash배열에 linked list를 통해 연결되어 있기 때문에 해당 linked list를 보면 해당 내용이 몇 번 트윗되었는지를 알 수 있습니다. 이를 이용하여 각 hash배열의 linked list를 살펴보아 가장 많이 트윗된 단어의 순위를 출력했습니다. O(n2)의 시간복잡도를 갖습니다.

• Top 5 most tweeted users (5)
	5.
	
• Find all users who mentioned a word (10)
	10.
	
• Find all users who are friend of the above user (5)
	5.
	
• Top 5 strongly connected components (10)
	10. Depth First Search 알고리즘을 사용하여 O(nlong)의 시간복잡도를 갖습니다. 나중에 입력받은 값을 먼저 출력하는 stack을 이용하여 구현하였고, 최대 5위까지의 Strongly connected components의 갯수와 대표 어미노드를 출력합니다.

• Find shortest path from a user (id) (10)
	10. 가장 짧은 경로에서 시작점이 되는 user id를 입력받아 출력합니다. d(거리)가 적은 노드를 우선적으로 출력하는 queue를 이용하여 구현하였으며, Dijkstra 알고리즘을 사용하여 O(nlogn)의 시간복잡도를 갖습니다. 정렬은 따로 할 필요 없이 enqueue과정에서 d를 기준으로 queue에 줄을 세우기 때문에 dequeue할 때 출력하면 자연스럽게 거리를 기준으로 오름차순으로 정렬된 상태로 출력됩니다.
