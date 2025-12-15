import java.util.*;
import java.util.Map.Entry;

public class TeamStrengthEstimator {

    // 存储队伍名称到索引的映射
    private final Map<String, Integer> teamIndex;

    // 存储比分矩阵，scores[i][j] 表示在队伍i和j的比赛中，队伍i获得的分数
    private double[][] scores;

    // 存储当前的实力估计值
    private double[] strengths;

    // 存储队伍名称列表，与索引对应
    private final List<String> teamNames;

    // 收敛阈值
    private static final double CONVERGENCE_THRESHOLD = 1e-8;

    // 最大迭代次数
    private static final int MAX_ITERATIONS = 1000;

    public TeamStrengthEstimator() {
        teamIndex = new HashMap<>();
        teamNames = new ArrayList<>();
    }

    /**
     * 添加一场比赛数据
     * @param team1 队伍1名称
     * @param team2 队伍2名称
     * @param score1 队伍1获得的分数
     * @param score2 队伍2获得的分数
     */
    public void addMatchResult(String team1, String team2, int score1, int score2) {
        // 确保两个队伍都在索引中
        if (!teamIndex.containsKey(team1)) {
            addNewTeam(team1);
        }
        if (!teamIndex.containsKey(team2)) {
            addNewTeam(team2);
        }

        int idx1 = teamIndex.get(team1);
        int idx2 = teamIndex.get(team2);

        // 更新比分矩阵
        scores[idx1][idx2] += score1;
        scores[idx2][idx1] += score2;

        // 重新估计实力
        estimateStrengths();

        // 输出当前估计的实力比例
        printStrengthRatios();
    }

    /**
     * 添加新队伍并扩展数据结构
     */
    private void addNewTeam(String teamName) {
        teamIndex.put(teamName, teamNames.size());
        teamNames.add(teamName);

        int newSize = teamNames.size();

        // 扩展比分矩阵
        double[][] newScores = new double[newSize][newSize];
        for (int i = 0; i < newSize - 1; i++) {
            System.arraycopy(scores[i], 0, newScores[i], 0, newSize - 1);
        }
        scores = newScores;

        // 扩展实力数组
        double[] newStrengths = new double[newSize];
        if (strengths != null) {
            System.arraycopy(strengths, 0, newStrengths, 0, newSize - 1);
        }
        newStrengths[newSize - 1] = 1.0; // 初始化为1
        strengths = newStrengths;
    }

    /**
     * 使用迭代算法估计实力
     */
    private void estimateStrengths() {
        int n = teamNames.size();
        if (n <= 1) return;

        // 初始化所有队伍实力为1
        Arrays.fill(strengths, 1.0);

        // 计算每个队伍的总得分
        double[] totalScores = new double[n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    totalScores[i] += scores[i][j];
                }
            }
        }

        // 迭代更新
        for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
            double[] newStrengths = new double[n];
            double maxChange = 0.0;

            // 计算每个队伍的新实力
            for (int i = 0; i < n; i++) {
                if (totalScores[i] == 0) {
                    newStrengths[i] = strengths[i];
                    continue;
                }

                double denominator = 0.0;
                for (int j = 0; j < n; j++) {
                    if (i != j && scores[i][j] + scores[j][i] > 0) {
                        denominator += (scores[i][j] + scores[j][i]) / (strengths[i] + strengths[j]);
                    }
                }

                if (denominator > 0) {
                    newStrengths[i] = totalScores[i] / denominator;
                } else {
                    newStrengths[i] = strengths[i];
                }
            }

            // 归一化（使最小实力为1）
            double minStrength = Double.MAX_VALUE;
            for (int i = 0; i < n; i++) {
                if (newStrengths[i] > 0 && newStrengths[i] < minStrength) {
                    minStrength = newStrengths[i];
                }
            }

            for (int i = 0; i < n; i++) {
                newStrengths[i] /= minStrength;
                // 计算最大变化
                maxChange = Math.max(maxChange, Math.abs(newStrengths[i] - strengths[i]));
            }

            strengths = newStrengths;

            // 检查收敛
            if (maxChange < CONVERGENCE_THRESHOLD) {
                System.out.println("迭代次数: " + (iter + 1));
                break;
            }
        }
    }

    /**
     * 输出当前估计的实力比例
     */
    private void printStrengthRatios() {
        System.out.println("\n当前估计的实力比例:");

        // 按实力大小排序
        List<Map.Entry<String, Double>> sortedStrengths = new ArrayList<>();
        for (int i = 0; i < teamNames.size(); i++) {
            sortedStrengths.add(new AbstractMap.SimpleEntry<>(teamNames.get(i), strengths[i]));
        }

        sortedStrengths.sort((a, b) -> Double.compare(b.getValue(), a.getValue()));

        // 找到最简单的整数比例（通过将所有值乘以适当的系数）
        double minVal = Double.MAX_VALUE;
        for (Entry<String, Double> entry : sortedStrengths) {
            if (entry.getValue() > 0 && entry.getValue() < minVal) {
                minVal = entry.getValue();
            }
        }

        // 输出原始比例
        for (Entry<String, Double> entry : sortedStrengths) {
            System.out.printf("%s: %.6f%n", entry.getKey(), entry.getValue());
        }
    }

    /**
     * 获取当前估计的实力比例（以映射形式）
     */
    public Map<String, Double> getStrengthRatios() {
        Map<String, Double> result = new HashMap<>();
        for (int i = 0; i < teamNames.size(); i++) {
            result.put(teamNames.get(i), strengths[i]);
        }
        return result;
    }

    /**
     * 控制台输入
     */
    public static void main(String[] args) {
        TeamStrengthEstimator estimator = new TeamStrengthEstimator();
        Scanner scanner = new Scanner(System.in);

        System.out.println("队伍实力比例估计算法");
        System.out.println("输入格式: 队伍1 队伍2 比分1 比分2");
        System.out.println("输入 'exit' 退出");
        System.out.println("示例: 湖人 勇士 120 115");

        while (true) {
            System.out.print("\n请输入比赛数据: ");
            String input = scanner.nextLine().trim();

            if (input.equalsIgnoreCase("exit")) {
                break;
            }

            String[] parts = input.split("\\s+");
            if (parts.length != 4) {
                System.out.println("输入格式错误，请重新输入");
                continue;
            }

            try {
                String team1 = parts[0];
                String team2 = parts[1];
                int score1 = Integer.parseInt(parts[2]);
                int score2 = Integer.parseInt(parts[3]);

                if (score1 < 0 || score2 < 0) {
                    System.out.println("比分不能为负数");
                    continue;
                }

                estimator.addMatchResult(team1, team2, score1, score2);

            } catch (NumberFormatException e) {
                System.out.println("比分必须是整数");
            }
        }

        scanner.close();
        System.out.println("程序结束");
    }

    /**
     * 示例：使用您的数据运行
     */
    public void run() {
        this.addMatchResult("安邦一队", "秉文", 28+25+15, 26+17+8);
        this.addMatchResult("安邦一队", "有训", 25+25+13, 18+10+15);
        this.addMatchResult("安邦一队", "毓琇", 25+25+11, 16+18+15);
        this.addMatchResult("安邦一队", "健雄一队", 16+25+13, 25+21+15);
        this.addMatchResult("安邦一队", "健雄二队", 25+25+15, 8+18+8);

        this.addMatchResult("安邦二队", "开甲", 9+11+5, 25+25+15);
        this.addMatchResult("安邦二队", "行知", 13+13+5, 25+25+15);
        this.addMatchResult("安邦二队", "健雄一队", 7+14+10, 25+25+15);

        this.addMatchResult("秉文", "开甲", 14+23+15, 25+25+11);
        this.addMatchResult("秉文", "毓琇", 26+23+9, 24+25+15);
        this.addMatchResult("秉文", "健雄一队", 15+15+9, 25+25+15);
        this.addMatchResult("秉文", "健雄二队", 25+25+15, 14+19+11);

        this.addMatchResult("开甲", "行知", 25+25+15, 13+19+12);
        this.addMatchResult("开甲", "有训", 25+25+6, 13+18+15);

        this.addMatchResult("行知", "毓琇", 13+25+11, 25+17+15);
        this.addMatchResult("行知", "健雄一队", 25+25+8, 16+22+15);
        this.addMatchResult("行知", "健雄二队", 25+23+15, 9+25+6);

        this.addMatchResult("有训", "毓琇", 17+14+13, 25+25+15);

        this.addMatchResult("毓琇", "健雄二队", 25+25+15, 11+11+5);
    }
}